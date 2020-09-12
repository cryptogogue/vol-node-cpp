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

    AbstractMiningMessengerClient&      mClient;
    string                              mMinerID;
    string                              mURL;
    shared_ptr < Block >                mBlock;

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
            
                json = json ? json->getObject ( "block" ) : NULL;
            
                if ( json ) {
                    shared_ptr < Block > block = make_shared < Block >();
                    FromJSONSerializer::fromJSON ( *block, *json );
                    this->mBlock = block;
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
    HTTPGetBlockTask ( AbstractMiningMessengerClient& client, string minerID, string url ) :
        mClient ( client ),
        Task ( "HTTP GET BLOCK" ),
        mMinerID ( minerID ),
        mURL ( url ) {
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
        task->mClient.receiveBlock ( task->mMinerID, task->mBlock );
    }
    pNf->release ();
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void HTTPMiningMessenger::AbstractMiningMessenger_requestBlock ( AbstractMiningMessengerClient& client, string minerID, string url, size_t height ) {

    string endpointURL;
    Format::write ( endpointURL, "%sblocks/%d/", url.c_str (), ( int )height );
    this->mTaskManager.start ( new HTTPGetBlockTask ( client, minerID, endpointURL ));
}

} // namespace Volition
