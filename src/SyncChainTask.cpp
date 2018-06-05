//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "AbstractHashable.h"
#include "SyncChainTask.h"

namespace Volition {

//================================================================//
// SyncChainTask
//================================================================//
    
//----------------------------------------------------------------//
SyncChainTask::SyncChainTask () :
    Task ( "SYNC CHAIN" ) {
}

//----------------------------------------------------------------//
SyncChainTask::~SyncChainTask () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void SyncChainTask::runTask () {

    Poco::URI uri ( "http://127.0.0.1:9091/" );
    std::string path ( uri.getPathAndQuery ());

    printf ( "%s\n", uri.getHost ().c_str ());
    printf ( "%d\n", ( int )uri.getPort ());

    Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
    Poco::Net::HTTPRequest request ( Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1 );
    Poco::Net::HTTPResponse response;
    session.sendRequest ( request );
    std::istream& rs = session.receiveResponse ( response );
    std::cout << response.getStatus () << " " << response.getReason () << std::endl;
    std::cout << rs.rdbuf ();
}

} // namespace Volition
