// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/simulation/Simulations.h>
#include <volition/TheContext.h>
#include <volition/TheWebMiner.h>
#include <volition/web-miner-api/HTTPRequestHandlerFactory.h>

using namespace Volition;

//----------------------------------------------------------------//
Poco::JSON::Object::Ptr httpGetJSON ( string url ) {

    try {

        Poco::URI uri ( url );
        std::string path ( uri.getPathAndQuery ());

        Poco::Net::HTTPRequest request ( Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1 );

        Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
        session.setTimeout ( Poco::Timespan ( 1, 0 ));
        session.sendRequest ( request );
        
        Poco::Net::HTTPResponse response;
        std::istream& responseStream = session.receiveResponse ( response );
        
        if ( response.getStatus () == Poco::Net::HTTPResponse::HTTP_OK ) {
            
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse ( responseStream );
            return result.extract < Poco::JSON::Object::Ptr >();
        }
    }
    catch ( Poco::Exception& exc ) {
        string message = exc.message ();
        LOG_F ( INFO, "%s", message.c_str ());
    }
    return NULL;
}

//----------------------------------------------------------------//
Poco::JSON::Object::Ptr httpPostJSON ( string url, string json ) {

    try {

        Poco::URI uri ( url );
        std::string path ( uri.getPathAndQuery ());

        Poco::Net::HTTPRequest request ( Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1 );
        request.setContentType ( "application/json" );
        request.setContentLength ( json.size ());

        Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
        session.setTimeout ( Poco::Timespan ( 1, 0 ));
        
        std::ostream& requestStream = session.sendRequest ( request );
        requestStream << json;
        
        Poco::Net::HTTPResponse response;
        std::istream& responseStream = session.receiveResponse ( response );
        
        if ( response.getStatus () == Poco::Net::HTTPResponse::HTTP_OK ) {
            
            Poco::JSON::Parser parser;
            Poco::Dynamic::Var result = parser.parse ( responseStream );
            return result.extract < Poco::JSON::Object::Ptr >();
        }
    }
    catch ( Poco::Exception& exc ) {
        string message = exc.message ();
        LOG_F ( INFO, "%s", message.c_str ());
    }
    return NULL;
}

//----------------------------------------------------------------//
string loadFileAsString ( string filename ) {

    if ( filename.size () && Poco::File ( filename ).exists ()) {
        fstream inStream;
        inStream.open ( filename, ios_base::in );
        return string ( istreambuf_iterator < char >( inStream ), {});
    }
    return string ();
}

//----------------------------------------------------------------//
void waitChainSize ( size_t size, long sleep = 100 ) {

    bool pass = false;
    do {
        Poco::Thread::sleep ( sleep );
        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        WebMiner& webMiner = scopedLock.getWebMiner ();
        pass = ( size <= webMiner.getChainSize ());
    } while ( !pass );
}

//----------------------------------------------------------------//
Poco::JSON::Object::Ptr waitServer ( string url ) {

   Poco::JSON::Object::Ptr json;
    do {
        json = httpGetJSON ( url );
    } while ( json.isNull ());
    
    return json;
}

//----------------------------------------------------------------//
TEST ( WebMiner, small_simulation ) {
    
    string genesis      = "genesis.signed";
    string keyfile      = "keys/pkey0.priv.json";
    int port            = 9090;
    string minerID      = to_string ( port );

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );

    {
        Volition::ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        Volition::WebMiner& webMiner = scopedLock.getWebMiner ();

        webMiner.setLazy ( true );
        webMiner.setSolo ( true );
        
        webMiner.loadKey ( keyfile );
        webMiner.loadGenesis ( genesis );
        webMiner.setMinerID ( minerID );
        webMiner.start ();
    }
    
    Poco::Net::HTTPServer server ( new WebMinerAPI::HTTPRequestHandlerFactory (), Poco::Net::ServerSocket ( port ), new Poco::Net::HTTPServerParams );
    server.start ();

    Poco::JSON::Object::Ptr json = httpGetJSON ( "http://127.0.0.1:9090/" );

    ASSERT_FALSE ( json.isNull ());
    ASSERT_TRUE ( json->getValue < string >( "type" ) == "VOL_MINING_NODE" );
    ASSERT_TRUE ( json->getValue < string >( "minerID" ) == minerID );

    json = httpPostJSON ( "http://127.0.0.1:9090/transactions", loadFileAsString ( "test/send-vol-from-9090-to-9091.json" ));
    
    ASSERT_FALSE ( json.isNull ());
    
    waitChainSize ( 2 );

    json = httpGetJSON ( "http://127.0.0.1:9090/accounts/9090" );

    ASSERT_FALSE ( json.isNull ());
    json = json->getObject ( "account" );
    
    ASSERT_FALSE ( json.isNull ());
    ASSERT_TRUE ( json->getValue < string >( "accountName" ) == "9090" );
    ASSERT_TRUE ( json->getValue < int >( "balance" ) == 999900 );
    ASSERT_TRUE ( json->getValue < int >( "nonce" ) == 1 );

    json = httpGetJSON ( "http://127.0.0.1:9090/accounts/9091" );

    ASSERT_FALSE ( json.isNull ());
    json = json->getObject ( "account" );
    
    ASSERT_FALSE ( json.isNull ());
    ASSERT_TRUE ( json->getValue < string >( "accountName" ) == "9091" );
    ASSERT_TRUE ( json->getValue < int >( "balance" ) == 1000100 );
    ASSERT_TRUE ( json->getValue < int >( "nonce" ) == 0 );

    server.stop ();

    TheWebMiner::get ().shutdown ();
    
    printf ( "SHUTDOWN!" );
}
