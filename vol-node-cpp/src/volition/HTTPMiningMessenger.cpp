// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/HTTPMiningMessenger.h>

namespace Volition {

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

        try {
        
            Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
            Poco::Net::HTTPRequest request ( Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1 );
            Poco::Net::HTTPResponse response;
            //session.setKeepAlive ( true );
            session.setTimeout ( Poco::Timespan ( 1, 0 ));
            session.sendRequest ( request );
            
            if ( response.getStatus () == Poco::Net::HTTPResponse::HTTP_OK ) {
            
                std::istream& jsonStream = session.receiveResponse ( response );
            
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
    }

public:

    //----------------------------------------------------------------//
    HTTPGetJSONTask ( TYPE userData, string url ) :
        mUserData ( userData ),
        mURL ( url ),
        Task ( "HTTP GET JSON" ) {
    }
    
    //----------------------------------------------------------------//
    ~HTTPGetJSONTask () {
    }
};

//================================================================//
// HTTPMiningMessenger
//================================================================//

//----------------------------------------------------------------//
HTTPMiningMessenger::HTTPMiningMessenger () :
    mTaskManager ( this->mTaskManagerThreadPool ) {
    
    this->mTaskManager.addObserver (
        Poco::Observer < HTTPMiningMessenger, Poco::TaskFinishedNotification > ( *this, &HTTPMiningMessenger::onTaskFinishedNotification )
    );
}

//----------------------------------------------------------------//
HTTPMiningMessenger::~HTTPMiningMessenger () {

    this->mTaskManager.cancelAll ();
    this->mTaskManager.joinAll ();
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::onTaskCancelledNotification ( Poco::TaskCancelledNotification* pNf ) {
    UNUSED ( pNf );
    
    HTTPGetJSONTask < MiningMessengerRequest >* task = dynamic_cast < HTTPGetJSONTask < MiningMessengerRequest >* >( pNf->task ());
    if ( task ) {
        const MiningMessengerRequest& request = task->mUserData;
        request.mClient->receiveError ( request );
    }
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::onTaskFailedNotification ( Poco::TaskFailedNotification* pNf ) {
    UNUSED ( pNf );
    
    HTTPGetJSONTask < MiningMessengerRequest >* task = dynamic_cast < HTTPGetJSONTask < MiningMessengerRequest >* >( pNf->task ());
    if ( task ) {
        const MiningMessengerRequest& request = task->mUserData;
        request.mClient->receiveError ( request );
    }
}

//----------------------------------------------------------------//
void HTTPMiningMessenger::onTaskFinishedNotification ( Poco::TaskFinishedNotification* pNf ) {

    HTTPGetJSONTask < MiningMessengerRequest >* task = dynamic_cast < HTTPGetJSONTask < MiningMessengerRequest >* >( pNf->task ());
    
    if ( task ) {
        
        const MiningMessengerRequest& request = task->mUserData;
        Poco::JSON::Object::Ptr json = task->mJSON;
        
        if ( json ) {
        
            switch ( request.mRequestType ) {
                
                case MiningMessengerRequest::REQUEST_BLOCK: {
                
                    Poco::JSON::Object::Ptr blockJSON = json ? json->getObject ( "block" ) : NULL;
                    if ( blockJSON ) {
                        shared_ptr < Block > block = make_shared < Block >();
                        FromJSONSerializer::fromJSON ( *block, *blockJSON );
                        request.mClient->receiveBlock ( request, block );
                    }
                    break;
                }
                case MiningMessengerRequest::REQUEST_HEADERS:
                case MiningMessengerRequest::REQUEST_PREV_HEADERS: {
                
                    Poco::JSON::Object::Ptr headersJSON = json ? json->getObject ( "headers" ) : NULL;
                    if ( headersJSON ) {
                
                        SerializableList < SerializableSharedConstPtr < BlockHeader >> headers;
                        FromJSONSerializer::fromJSON ( headers, *headersJSON );
                        
                        SerializableList < SerializableSharedConstPtr < BlockHeader >>::const_iterator headersIt = headers.cbegin ();
                        for ( ; headersIt != headers.cend (); ++headersIt ) {
                            request.mClient->receiveHeader ( request, *headersIt );
                        }
                    }
                    break;
                }
                case MiningMessengerRequest::REQUEST_MINER: {
                    break;
                }
                case MiningMessengerRequest::REQUEST_MINER_URLS: {
                
                    Poco::JSON::Object::Ptr minerListJSON = json ? json->getObject ( "miners" ) : NULL;
                    if ( minerListJSON ) {
                
                        SerializableList < string > minerList;
                        FromJSONSerializer::fromJSON ( minerList, *minerListJSON );
                        
                        SerializableList < string >::const_iterator minerListIt = minerList.cbegin ();
                        for ( ; minerListIt != minerList.cend (); ++minerListIt ) {
                            request.mClient->receiveMinerURL ( request, *minerListIt );
                        }
                    }
                    break;
                }
                default:
                    request.mClient->receiveError ( request );
                    break;
            }
        }
        else {
            request.mClient->receiveError ( request );
        }
    }
    pNf->release ();
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void HTTPMiningMessenger::AbstractMiningMessenger_request ( const MiningMessengerRequest& request ) {

    string url;

    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_BLOCK:
            Format::write ( url, "%sconsensus/blocks/%s", request.mBaseURL.c_str (), request.mBlockDigest.toHex ().c_str ());
            break;
    
        case MiningMessengerRequest::REQUEST_HEADERS:
            Format::write ( url, "%sconsensus/headers", request.mBaseURL.c_str ());
            break;
        
        case MiningMessengerRequest::REQUEST_PREV_HEADERS:
            Format::write ( url, "%sconsensus/headers?height=%llu", request.mBaseURL.c_str (), request.mHeight );
            break;
        
        case MiningMessengerRequest::REQUEST_MINER:
            Format::write ( url, "%snode", request.mBaseURL.c_str ());
            break;
        
        case MiningMessengerRequest::REQUEST_MINER_URLS:
            Format::write ( url, "%sminers?sample=random", request.mBaseURL.c_str ());
            break;
        
        default:
            assert ( false );
            break;
    }

    this->mTaskManager.start ( new HTTPGetJSONTask < MiningMessengerRequest >( request, url ));
}

} // namespace Volition
