// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Chain.h>
#include <volition/SyncChainTask.h>

namespace Volition {

//================================================================//
// SyncChainTask
//================================================================//

//----------------------------------------------------------------//
string SyncChainTask::getMinerID () const {

    return this->mMinerID;
}

//----------------------------------------------------------------//
string SyncChainTask::getURL () const {

    return this->mURL;
}

//----------------------------------------------------------------//
unique_ptr < Chain > SyncChainTask::moveChain () {

    return move ( this->mChain );
}

//----------------------------------------------------------------//
SyncChainTask::SyncChainTask ( string minerID, string url ) :
    Task ( "SYNC CHAIN" ),
    mMinerID ( minerID ),
    mURL ( url ) {
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

    Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
    Poco::Net::HTTPRequest request ( Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1 );
    Poco::Net::HTTPResponse response;
    session.sendRequest ( request );
    std::istream& rs = session.receiveResponse ( response );
    
    if ( response.getStatus () == Poco::Net::HTTPResponse::HTTP_OK ) {
        this->mChain = make_unique < Chain >();
        FromJSONSerializer::fromJSON ( *this->mChain, rs );
    }
}

} // namespace Volition
