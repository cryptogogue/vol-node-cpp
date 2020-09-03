// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>
#include <volition/serialization/Serialization.h>
#include <volition/simulation/Simulations.h>
#include <volition/TheContext.h>
#include <volition/web-miner-api/HTTPRequestHandlerFactory.h>
#include <volition/WebMiner.h>

using namespace Volition;

//----------------------------------------------------------------//
Poco::JSON::Object::Ptr http ( string url, const std::string& method, string json ) {

    try {

        //bool post = ( method == Poco::Net::HTTPRequest::HTTP_GET );

        Poco::URI uri ( url );
        std::string path ( uri.getPathAndQuery ());

        Poco::Net::HTTPRequest request ( method, path, Poco::Net::HTTPMessage::HTTP_1_1 );
        request.setContentType ( "application/json" );
        request.setContentLength ( json.size ());

        Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
        session.setTimeout ( Poco::Timespan ( 10, 0 ));
        
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
Poco::JSON::Object::Ptr httpGetJSON ( string url ) {

    return http ( url, Poco::Net::HTTPRequest::HTTP_GET, "" );
}

//----------------------------------------------------------------//
Poco::JSON::Object::Ptr httpPostJSON ( string url, string json ) {

    return http ( url, Poco::Net::HTTPRequest::HTTP_POST, json );
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
void waitChainSize ( shared_ptr < WebMiner > webMiner, size_t size, long sleep = 100 ) {

    bool pass = false;
    do {
        Poco::Thread::sleep ( sleep );
        ScopedWebMinerLock scopedLock ( webMiner );
        pass = ( size <= webMiner->getChainSize ());
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
TEST ( WebMiner, asset_transformations ) {

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );

    shared_ptr < WebMiner > webMiner = make_shared < WebMiner >();
        
    webMiner->setLazy ( true );
    webMiner->setSolo ( true );
        
    webMiner->loadKey ( "keys/pkey0.priv.json" );
    webMiner->loadGenesis ( "genesis.signed" );
    webMiner->setMinerID ( "9090" );
    webMiner->start ();
    
    Poco::Net::HTTPServer server ( new Volition::WebMinerAPIFactory ( webMiner ), Poco::Net::ServerSocket ( 9090 ), new Poco::Net::HTTPServerParams );
    server.start ();

    Poco::JSON::Object::Ptr json = httpGetJSON ( "http://127.0.0.1:9090/" );
    ASSERT_FALSE ( json.isNull ());
    
    ASSERT_TRUE ( json->getValue < string >( "type" ) == "VOL_MINING_NODE" );
    ASSERT_TRUE ( json->getValue < string >( "minerID" ) == "9090" );

    json = httpPostJSON ( "http://127.0.0.1:9090/transactions", loadFileAsString ( "test/publish-test-schema.json" ));
    ASSERT_FALSE ( json.isNull ());
    waitChainSize ( webMiner, 2 );

    json = httpGetJSON ( "http://127.0.0.1:9090/accounts/9090/inventory" );
    ASSERT_FALSE ( json.isNull ());
    
    json = json->getObject ( "inventory" );
    ASSERT_FALSE ( json.isNull ());
    
    Inventory inventory;
    FromJSONSerializer::fromJSON ( inventory, *json );
    
    ASSERT_TRUE ( inventory.mAssets.size () == 1 );
    ASSERT_TRUE ( inventory.mAssets.front ().mClassName == "pack" );
    ASSERT_TRUE ( inventory.mAssets.front ().mQuantity == 1000 );

    json = httpPostJSON ( "http://127.0.0.1:9090/transactions", loadFileAsString ( "test/open-pack-9090.json" ));
    ASSERT_FALSE ( json.isNull ());
    
    json = httpPostJSON ( "http://127.0.0.1:9090/test/extendChain", "{}" );
    ASSERT_FALSE ( json.isNull ());
    
    waitChainSize ( webMiner, 3 );
    
    json = httpPostJSON ( "http://127.0.0.1:9090/test/extendChain", "{}" );
    ASSERT_FALSE ( json.isNull ());
    
    waitChainSize ( webMiner, 4 );

    server.stop ();
    
    webMiner->shutdown ();
}

//----------------------------------------------------------------//
TEST ( WebMiner, small_simulation ) {

    TheContext::get ().setScoringMode ( TheContext::ScoringMode::INTEGER );

    shared_ptr < WebMiner > webMiner = make_shared < WebMiner >();

    webMiner->setLazy ( true );
    webMiner->setSolo ( true );
        
    webMiner->loadKey ( "keys/pkey0.priv.json" );
    webMiner->loadGenesis ( "genesis.signed" );
    webMiner->setMinerID ( "9090" );
    webMiner->start ();
    
    Poco::Net::HTTPServer server ( new WebMinerAPI::HTTPRequestHandlerFactory (), Poco::Net::ServerSocket ( 9090 ), new Poco::Net::HTTPServerParams );
    server.start ();

    Poco::JSON::Object::Ptr json = httpGetJSON ( "http://127.0.0.1:9090/" );
    ASSERT_FALSE ( json.isNull ());
    
    ASSERT_TRUE ( json->getValue < string >( "type" ) == "VOL_MINING_NODE" );
    ASSERT_TRUE ( json->getValue < string >( "minerID" ) == "9090" );

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

    webMiner->shutdown ();
}
