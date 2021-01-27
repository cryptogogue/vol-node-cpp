// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/HTTPMiningMessenger.h>
#include <volition/UnsecureRandom.h>

namespace Volition {

//================================================================//
// HTTPMiningMessengerRequestQueue
//================================================================//

//----------------------------------------------------------------//
HTTPMiningMessengerRequestQueue::HTTPMiningMessengerRequestQueue () :
    mActiveCount ( 0 ),
    mRawWeight ( 0 ),
    mWeight ( 0.0 ) {
}

//----------------------------------------------------------------//
HTTPMiningMessengerRequestQueue::~HTTPMiningMessengerRequestQueue () {
}

//----------------------------------------------------------------//
void HTTPMiningMessengerRequestQueue::pushRequest ( const MiningMessengerRequest& request ) {

    this->mPending.push_back ( request );
}

//================================================================//
// HTTPGetJSONTask
//================================================================//
template < typename TYPE >
class HTTPGetJSONTask :
    public Poco::Task {
private:

    friend class HTTPMiningMessenger;

    TYPE                                mUserData;
    string                              mURL;
    Poco::JSON::Object::Ptr             mJSON;

    //----------------------------------------------------------------//
    void runTask () override {
    
        Poco::URI uri ( this->mURL );
        std::string path ( uri.getPathAndQuery ());
        
        Poco::Net::HTTPClientSession* session = NULL;
        
        try {

            const Poco::Net::Context::Ptr context = new Poco::Net::Context ( Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH" );
                    
            // TODO: this is so fucking gross, I hate POCO so fucking much
            if ( uri.getScheme () == "https" ) {
                session = new Poco::Net::HTTPSClientSession ( uri.getHost (), uri.getPort ());
            }
            else {
                session = new Poco::Net::HTTPClientSession ( uri.getHost (), uri.getPort ());
            }
                    
            Poco::Net::HTTPRequest request ( Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1 );
            session->setKeepAlive ( true );
//            session->setTimeout ( Poco::Timespan ( 30, 0 ));
            session->sendRequest ( request );
            
            Poco::Net::HTTPResponse response;
            std::istream& jsonStream = session->receiveResponse ( response );
            
            if ( response.getStatus () == Poco::Net::HTTPResponse::HTTP_OK ) {
                
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var result = parser.parse ( jsonStream );
                this->mJSON = result.extract < Poco::JSON::Object::Ptr >();
            }
        }
        catch ( Poco::Exception& exc ) {
            string msg = exc.message ();
            if ( msg.size () > 0 ) {
                LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s", exc.message ().c_str ());
            }
        }
        
        if ( session ) {
            delete ( session );
        }
    }

public:

    //----------------------------------------------------------------//
    HTTPGetJSONTask ( TYPE userData, string url ) :
        Task ( "HTTP GET JSON" ),
        mUserData ( userData ),
        mURL ( url ) {
    }
    
    //----------------------------------------------------------------//
    ~HTTPGetJSONTask () {
    }
};

//================================================================//
// HTTPMiningMessenger
//================================================================//

//----------------------------------------------------------------//
void HTTPMiningMessenger::completeRequest ( const MiningMessengerRequest& request ) {
    Poco::ScopedLock < Poco::Mutex > lock ( this->mMutex );

    this->mQueues [ this->getQueueIndex ( request )].mActiveCount--;
    this->mTotalActive--;
    
    this->pumpQueues ();
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::deserailizeHeaderList ( list < shared_ptr < const BlockHeader >>& responseHeaders, Poco::JSON::Array::Ptr headersJSON ) {

    assert ( headersJSON );

    SerializableList < SerializableSharedConstPtr < BlockHeader >> headers;
    FromJSONSerializer::fromJSON ( headers, *headersJSON );
    
    SerializableList < SerializableSharedConstPtr < BlockHeader >>::const_iterator headersIt = headers.cbegin ();
    for ( ; headersIt != headers.cend (); ++headersIt ) {
        responseHeaders.push_back ( *headersIt );
    }
}

//----------------------------------------------------------------//
size_t HTTPMiningMessenger::getQueueIndex ( const MiningMessengerRequest& request ) const {

    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_BLOCK:
            return BLOCK_QUEUE_INDEX;

        case MiningMessengerRequest::REQUEST_EXTEND_NETWORK:
            return TOPOLOGY_QUEUE_INDEX;
    
        case MiningMessengerRequest::REQUEST_HEADERS:
            return HEADER_QUEUE_INDEX;
        
        case MiningMessengerRequest::REQUEST_MINER_INFO:
            return INFO_QUEUE_INDEX;

        default:
            break;
    }
    
    assert ( false );
    return TOTAL_QUEUES;
}

//----------------------------------------------------------------//
size_t HTTPMiningMessenger::getQueueRawWeight ( size_t index ) const {

    switch ( index ) {
        
        case BLOCK_QUEUE_INDEX:         return BLOCK_QUEUE_WEIGHT;
        case HEADER_QUEUE_INDEX:        return HEADER_QUEUE_WEIGHT;
        case INFO_QUEUE_INDEX:          return INFO_QUEUE_WEIGHT;
        case TOPOLOGY_QUEUE_INDEX:      return TOPOLOGY_QUEUE_WEIGHT;

        default: break;
    }
    
    assert ( false );
    return 0;
}

//----------------------------------------------------------------//
string HTTPMiningMessenger::getRequestURL ( const MiningMessengerRequest& request ) const {

    string url;

    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_BLOCK:
            Format::write ( url, "%s/consensus/blocks/%s", request.mMinerURL.c_str (), request.mBlockDigest.toHex ().c_str ());
            break;
    
        case MiningMessengerRequest::REQUEST_EXTEND_NETWORK:
            Format::write ( url, "%s/miners?sample=random", request.mMinerURL.c_str ());
            break;
    
        case MiningMessengerRequest::REQUEST_HEADERS:
            Format::write ( url, "%s/consensus/headers?height=%llu", request.mMinerURL.c_str (), request.mHeight );
            break;
        
        case MiningMessengerRequest::REQUEST_MINER_INFO:
            Format::write ( url, "%s/node", request.mMinerURL.c_str ());
            break;
        
        default:
            assert ( false );
            break;
    }
    
    return url;
}

//----------------------------------------------------------------//
HTTPMiningMessenger::HTTPMiningMessenger () :
    mTaskManager ( this->mThreadPool ),
    mTotalPending ( 0 ),
    mTotalActive ( 0 ) {
    
    this->mTaskManager.addObserver (
        Poco::Observer < HTTPMiningMessenger, Poco::TaskFinishedNotification > ( *this, &HTTPMiningMessenger::onTaskFinishedNotification )
    );
    
    this->mTaskManager.addObserver (
        Poco::Observer < HTTPMiningMessenger, Poco::TaskCancelledNotification > ( *this, &HTTPMiningMessenger::onTaskCancelledNotification )
    );

    this->mTaskManager.addObserver (
        Poco::Observer < HTTPMiningMessenger, Poco::TaskFailedNotification > ( *this, &HTTPMiningMessenger::onTaskFailedNotification )
    );
    
    size_t totalWeight = 0;
    
    for ( size_t i = 0; i < TOTAL_QUEUES; ++i ) {
        HTTPMiningMessengerRequestQueue& queue = this->mQueues [ i ];
        queue.mRawWeight = this->getQueueRawWeight ( i );
        totalWeight += queue.mRawWeight;
    }
    
    for ( size_t i = 0; i < TOTAL_QUEUES; ++i ) {
        HTTPMiningMessengerRequestQueue& queue = this->mQueues [ i ];
        queue.mWeight = (( double )queue.mRawWeight / ( double )totalWeight );
    }
}

//----------------------------------------------------------------//
HTTPMiningMessenger::~HTTPMiningMessenger () {

    this->mTaskManager.cancelAll ();
    this->mTaskManager.joinAll ();
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::onTaskCancelledNotification ( Poco::TaskCancelledNotification* pNf ) {
    
    Poco::TaskManager::TaskPtr taskPtr = pNf->task (); // TODO: this bullshit right here. pNf->task () increments the ref count. because of course it does.
    HTTPGetJSONTask < MiningMessengerRequest >* task = dynamic_cast < HTTPGetJSONTask < MiningMessengerRequest >* >( taskPtr.get ());
    
    assert ( task );
    
    const MiningMessengerRequest& request = task->mUserData;
    this->enqueueErrorResponse ( request );
    this->completeRequest ( request );
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::onTaskFailedNotification ( Poco::TaskFailedNotification* pNf ) {
        
    Poco::TaskManager::TaskPtr taskPtr = pNf->task (); // TODO: this bullshit right here. pNf->task () increments the ref count. because of course it does.
    HTTPGetJSONTask < MiningMessengerRequest >* task = dynamic_cast < HTTPGetJSONTask < MiningMessengerRequest >* >( taskPtr.get ());
    
    const MiningMessengerRequest& request = task->mUserData;
    this->enqueueErrorResponse ( request );
    this->completeRequest ( request );
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::onTaskFinishedNotification ( Poco::TaskFinishedNotification* pNf ) {

    Poco::TaskManager::TaskPtr taskPtr = pNf->task (); // TODO: this bullshit right here. pNf->task () increments the ref count. because of course it does.
    HTTPGetJSONTask < MiningMessengerRequest >* task = dynamic_cast < HTTPGetJSONTask < MiningMessengerRequest >* >( taskPtr.get ());
    
    assert ( task );
        
    const MiningMessengerRequest& request = task->mUserData;
    Poco::JSON::Object::Ptr json = task->mJSON;
    
    if ( json ) {
    
        switch ( request.mRequestType ) {
            
            case MiningMessengerRequest::REQUEST_BLOCK: {
            
                shared_ptr < Block > block;
                
                Poco::JSON::Object::Ptr blockJSON = json ? json->getObject ( "block" ) : NULL;
                if ( blockJSON ) {
                    block = make_shared < Block >();
                    FromJSONSerializer::fromJSON ( *block, *blockJSON );
                }
                this->enqueueBlockResponse ( request, block );
                
                break;
            }
            
            case MiningMessengerRequest::REQUEST_EXTEND_NETWORK: {
            
                Poco::JSON::Array::Ptr minerListJSON = json ? json->getArray ( "miners" ) : NULL;
                if ( minerListJSON ) {
                    SerializableSet < string > minerSet;
                    FromJSONSerializer::fromJSON ( minerSet, *minerListJSON );
                    this->enqueueExtendNetworkResponse ( request, minerSet );
                }
                break;
            }
            
            case MiningMessengerRequest::REQUEST_HEADERS: {
            
                Poco::JSON::Array::Ptr headersJSON = json ? json->getArray ( "headers" ) : NULL;
                if ( headersJSON ) {
                    list < shared_ptr < const BlockHeader >> responseHeaders;
                    HTTPMiningMessenger::deserailizeHeaderList ( responseHeaders, headersJSON );
                    this->enqueueHeadersResponse ( request, responseHeaders );
                }
                break;
            }
            
            case MiningMessengerRequest::REQUEST_MINER_INFO: {
            
                Poco::JSON::Object::Ptr nodeJSON = json ? json->getObject ( "node" ) : NULL;
                if ( nodeJSON ) {
                    string minerID  = nodeJSON->optValue < string >( "minerID", "" );
                    this->enqueueMinerInfoResponse ( request, minerID, request.mMinerURL );
                }
                break;
            }
            
            default:
                this->enqueueErrorResponse ( request );
                break;
        }
    }
    else {
        this->enqueueErrorResponse ( request );
    }
    
    this->completeRequest ( request );
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::pumpQueues () {

    // keep going until all available resources have been used.
    while ( this->mTotalPending && this->mThreadPool.available ()) {

        // we measure utilization as a percent of total resources, so that is active plus available.
        size_t totalResources = this->mTotalActive + ( size_t )this->mThreadPool.available ();

        set < HTTPMiningMessengerRequestQueue* > under;
        set < HTTPMiningMessengerRequestQueue* > over;
        
        // loop through the queues and calculate utilization percentages. sort them into two
        // pools: under their target percent or over.
        for ( size_t i = 0; i < TOTAL_QUEUES; ++i ) {
            
            HTTPMiningMessengerRequestQueue& queue = this->mQueues [ i ];
            if ( queue.mPending.size () == 0 ) continue; // ignore queues with nothing pending.
            
            // this is the current utilization percentage the queue's active requests againt total resources.
            double weight = (( double )queue.mActiveCount / ( double )totalResources );
            
            // sort the queue into either underserved or overserved.
            (( weight < queue.mWeight ) ? under : over ).insert ( &queue );
        }

        // if there are any underserved queues, choose those instead of overserved queues.
        set < HTTPMiningMessengerRequestQueue* >& queues = under.size () ? under : over;
        
        // if mTotalPending is not zero, there *must* be pending requests, which means this *can't* be empty.
        assert ( queues.size () != 0 );
        
        // now, while there are resources available, randomly select a queue, send the request and remove it.
        while (( this->mThreadPool.available () > 0 ) && queues.size ()) {
            set < HTTPMiningMessengerRequestQueue* >::iterator queueIt = queues.begin ();
            std::advance ( queueIt, ( long )( UnsecureRandom::get ().random ( 0, queues.size () - 1 )));
            this->sendRequest ( **queueIt );
            queues.erase ( queueIt );
        }
        
        // all of the queues in the set have been serviced; if there are still pending requests and
        // resources available, rinse and repeat.
    }
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::sendRequest ( HTTPMiningMessengerRequestQueue& queue ) {

    assert ( queue.mPending.size ());

    // pick a pending request at random
    list < MiningMessengerRequest >::iterator requestIt = queue.mPending.begin ();
    std::advance ( requestIt, ( long )( UnsecureRandom::get ().random ( 0, queue.mPending.size () - 1 )));

    assert ( requestIt != queue.mPending.end ());

    // send the request
    MiningMessengerRequest& request = *requestIt;
    string url = this->getRequestURL ( request );
    this->mTaskManager.start ( new HTTPGetJSONTask < MiningMessengerRequest >( request, url ));

    // remove it from the queue and update the counts
    queue.mPending.erase ( requestIt );
    queue.mActiveCount++;
    
    this->mTotalActive++;
    this->mTotalPending--;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void HTTPMiningMessenger::AbstractMiningMessenger_await () {

    this->mThreadPool.joinAll ();
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::AbstractMiningMessenger_sendRequest ( const MiningMessengerRequest& request ) {

    Poco::ScopedLock < Poco::Mutex > lock ( this->mMutex );

    this->mQueues [ this->getQueueIndex ( request )].pushRequest ( request );
    this->mTotalPending++;
    this->pumpQueues ();
}

} // namespace Volition
