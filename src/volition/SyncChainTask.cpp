// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// SyncChainTask
//================================================================//

//----------------------------------------------------------------//
const Chain& SyncChainTask::getChain () const {

    return this->mChain;
}

//----------------------------------------------------------------//
string SyncChainTask::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
string SyncChainTask::getURL () const {

    return this->mURL;
}

//----------------------------------------------------------------//
bool SyncChainTask::isValid () const {

    return this->mIsValid;
}

//----------------------------------------------------------------//
SyncChainTask::SyncChainTask ( string minerID, string url ) :
    Task ( "SYNC CHAIN" ),
    mMinerID ( minerID ),
    mURL ( url ),
    mIsValid ( false ) {
}

//----------------------------------------------------------------//
SyncChainTask::~SyncChainTask () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void SyncChainTask::runTask () {

    Poco::URI uri ( this->mURL );
    std::string path ( uri.getPathAndQuery ());

    try {
        Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
        Poco::Net::HTTPRequest request ( Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1 );
        Poco::Net::HTTPResponse response;
        //session.setKeepAlive ( true );
        session.setTimeout ( Poco::Timespan ( 1, 0 ));
        session.sendRequest ( request );
        std::istream& rs = session.receiveResponse ( response );
        
        if ( response.getStatus () == Poco::Net::HTTPResponse::HTTP_OK ) {
            FromJSONSerializer::fromJSON ( this->mChain, rs );
            this->mIsValid = true;
        }
    }
    catch ( Poco::Exception& exc ) {
        LOG_F ( INFO, "%s", exc.message ().c_str ());
    }
}

} // namespace Volition
