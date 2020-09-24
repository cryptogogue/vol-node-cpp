// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/HTTPMiningMessenger.h>

namespace Volition {

//================================================================//
// HTTPGetBlockTask
//================================================================//
class HTTPGetBlockTask :
    public Poco::Task {
private:

    friend class HTTPMiningMessenger;

    MiningMessengerRequest              mRequest;
    string                              mURL;
    shared_ptr < const BlockHeader >    mBlockHeader;
    shared_ptr < const Block >          mBlock;

    //----------------------------------------------------------------//
    void runTask () override {
    
        Poco::URI uri ( this->mURL );
        std::string path ( uri.getPathAndQuery ());

        this->mBlock = NULL;

        try {
            Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
            Poco::Net::HTTPRequest request ( Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1 );
            Poco::Net::HTTPResponse response;
            //session.setKeepAlive ( true );
            session.setTimeout ( Poco::Timespan ( 1, 0 ));
            session.sendRequest ( request );
            
            if ( response.getStatus () == Poco::Net::HTTPResponse::HTTP_OK ) {
            
                std::istream& jsonStream = session.receiveResponse ( response );
            
                //string jsonString ( istreambuf_iterator < char >( jsonStream ), {});
                //LGN_LOG ( VOL_FILTER_ROOT, INFO, "JSON: %s", jsonString ());
            
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var result = parser.parse ( jsonStream );
                Poco::JSON::Object::Ptr json = result.extract < Poco::JSON::Object::Ptr >();
            
                Poco::JSON::Object::Ptr blockJSON = json ? json->getObject ( "block" ) : NULL;
                
                if ( blockJSON ) {
                    shared_ptr < Block > block = make_shared < Block >();
                    FromJSONSerializer::fromJSON ( *block, *blockJSON );
                    this->mBlock        = block;
                    this->mBlockHeader  = block;
                }
                else {
            
                    Poco::JSON::Object::Ptr headerJSON = json ? json->getObject ( "header" ) : NULL;
                    
                    if ( headerJSON ) {
                        shared_ptr < BlockHeader > header = make_shared < BlockHeader >();
                        FromJSONSerializer::fromJSON ( *header, *headerJSON );
                        this->mBlockHeader  = header;
                    }
                }
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
    HTTPGetBlockTask ( const MiningMessengerRequest& request ) :
        mRequest ( request ),
        Task ( "HTTP GET BLOCK" ) {
        
        switch ( request.mRequestType ) {
            
            case MiningMessengerRequest::REQUEST_BLOCK:
                Format::write ( this->mURL, "%sblocks/%s", request.mBaseURL.c_str (), request.mBlockDigest.toHex ().c_str ());
                break;
        
            case MiningMessengerRequest::REQUEST_HEADER:
                Format::write ( this->mURL, "%schain/%d", request.mBaseURL.c_str (), ( int )request.mHeight );
                break;
            
            default:
                assert ( false );
                break;
        }
    }
    
    //----------------------------------------------------------------//
    ~HTTPGetBlockTask () {
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
void HTTPMiningMessenger::onTaskFinishedNotification ( Poco::TaskFinishedNotification* pNf ) {

    HTTPGetBlockTask* task = dynamic_cast < HTTPGetBlockTask* >( pNf->task ());
    if ( task ) {
        const MiningMessengerRequest& request = task->mRequest;
        request.mClient->receive ( request, task->mBlockHeader, task->mBlock );
    }
    pNf->release ();
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void HTTPMiningMessenger::AbstractMiningMessenger_request ( const MiningMessengerRequest& request ) {

    this->mTaskManager.start ( new HTTPGetBlockTask ( request ));
}

} // namespace Volition
