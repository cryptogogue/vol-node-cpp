// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <gtest/gtest.h>

#include <volition/MinerAPIFactory.h>
#include <volition/serialization/Serialization.h>
#include <volition/simulation/SimMiner.h>
#include <volition/simulation/SimMiner.h>
#include <volition/Transaction.h>
#include <volition/Transactions.h>
#include <volition/web-miner-api/ConsensusBlockDetailsHandler.h>
#include <volition/web-miner-api/ConsensusBlockHeaderListHandler.h>

using namespace Volition;
using namespace Simulation;
using namespace Transactions;
using namespace WebMinerAPI;

//----------------------------------------------------------------//
Poco::JSON::Object::Ptr                     http                                    ( string url, const std::string& method, string json );
Poco::JSON::Object::Ptr                     httpGetJSON                             ( string url );
Poco::JSON::Object::Ptr                     httpPostJSON                            ( string url, string json );
Poco::JSON::Object::Ptr                     httpPutJSON                             ( string url, string json );
string                                      loadFileAsString                        ( string filename );
shared_ptr < Transaction >                  makeTransaction                         (
                                                                                        shared_ptr < AbstractTransactionBody > body,
                                                                                        string uuid,
                                                                                        const CryptoKeyPair& key,
                                                                                        string accountName,
                                                                                        string keyName              = "master",
                                                                                        u64 nonce                   = 0,
                                                                                        u64 gratuity                = 0,
                                                                                        u64 maxHeight               = ( u64 )-1,
                                                                                        time_t recordBy             = ( time_t )-1
                                                                                    );
shared_ptr < AbstractTransactionBody >      makeTransactionBody_SendVOL             ( string to, u64 amount );

//----------------------------------------------------------------//
Poco::JSON::Object::Ptr http ( string url, const std::string& method, string json ) {

    try {

        //bool post = ( method == Poco::Net::HTTPRequest::HTTP_GET );

        Poco::URI uri ( url );
        std::string path ( uri.getPathAndQuery ());

        Poco::Net::HTTPRequest request ( method, path, Poco::Net::HTTPMessage::HTTP_1_1 );
        request.setContentType ( "application/json" );
        request.setContentLength (( std::streamsize )json.size ());

        Poco::Net::HTTPClientSession session ( uri.getHost (), uri.getPort ());
//        session.setTimeout ( Poco::Timespan ( 10, 0 ));

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
Poco::JSON::Object::Ptr httpPutJSON ( string url, string json ) {

    return http ( url, Poco::Net::HTTPRequest::HTTP_PUT, json );
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
shared_ptr < Transaction > makeTransaction (
    shared_ptr < AbstractTransactionBody > body,
    string uuid,
    const CryptoKeyPair& key,
    string accountName,
    string keyName,
    u64 nonce,
    u64 gratuity,
    u64 maxHeight,
    time_t recordBy
) {

    TransactionMaker maker;
    maker.setAccountName ( accountName );
    maker.setGratuity ( gratuity );
    maker.setNonce ( nonce );
    maker.setKeyName ( keyName );
    
    body->setMaker ( maker );
    body->setUUID ( uuid );
    body->setRecordBy ( recordBy );
    body->setMaxHeight ( maxHeight );

    shared_ptr < Transaction > transaction = make_shared < Transaction >();
    transaction->setBody ( body );
    transaction->sign ( key );

    return transaction;
}

//----------------------------------------------------------------//
shared_ptr < AbstractTransactionBody > makeTransactionBody_SendVOL ( string to, u64 amount ) {

    shared_ptr < SendVOL > body = make_shared < SendVOL >();
    body->mAmount       = amount;
    body->mAccountName  = to;

    return body;
}

//----------------------------------------------------------------//
TEST ( WebMiner, asset_transformations ) {

    shared_ptr < SimMiner > miner = make_shared < SimMiner >();
        
    miner->loadKey ( "keys/9090.keypair.json" );
    miner->loadGenesisBlock ( "genesis" );
    miner->setMinerID ( "9090" );
    miner->affirmKey ();
    miner->affirmVisage ();
    
    Poco::Net::HTTPServer server ( new MinerAPIFactory ( miner ), Poco::Net::ServerSocket ( 9090 ), new Poco::Net::HTTPServerParams );
    server.start ();

    Poco::JSON::Object::Ptr json = httpGetJSON ( "http://127.0.0.1:9090/" );
    ASSERT_FALSE ( json.isNull ());
    
    ASSERT_TRUE ( json->getValue < string >( "type" ) == "VOL_MINING_NODE" );
    ASSERT_TRUE ( json->getValue < string >( "minerID" ) == "9090" );

    // TODO: restore test with progrmatic transactions

//    json = httpPutJSON ( "http://127.0.0.1:9090/accounts/9090/transactions/068312f0-8127-41f6-b032-57cb81db5182", loadFileAsString ( "test/publish-test-schema.json" ));
//    ASSERT_FALSE ( json.isNull ());
//    waitChainSize ( miner, 2 );

//    json = httpGetJSON ( "http://127.0.0.1:9090/accounts/9090/inventory" );
//    ASSERT_FALSE ( json.isNull ());
//
//    json = json->getObject ( "inventory" );
//    ASSERT_FALSE ( json.isNull ());
//
//    Inventory inventory;
//    FromJSONSerializer::fromJSON ( inventory, *json );
//
//    ASSERT_TRUE ( inventory.mAssets.size () == 1 );
//    ASSERT_TRUE ( inventory.mAssets.front ().mClassName == "pack" );
//    ASSERT_TRUE ( inventory.mAssets.front ().mQuantity == 1000 );
//
//    json = httpPostJSON ( "http://127.0.0.1:9090/transactions", loadFileAsString ( "test/open-pack-9090.json" ));
//    ASSERT_FALSE ( json.isNull ());
//
//    json = httpPostJSON ( "http://127.0.0.1:9090/test/extendChain", "{}" );
//    ASSERT_FALSE ( json.isNull ());
//
//    waitChainSize ( minerActivity, 3 );
//
//    json = httpPostJSON ( "http://127.0.0.1:9090/test/extendChain", "{}" );
//    ASSERT_FALSE ( json.isNull ());
//
//    waitChainSize ( minerActivity, 4 );

    server.stop ();
}

//----------------------------------------------------------------//
TEST ( WebMiner, basic_api ) {

    shared_ptr < SimMiner > miner = make_shared < SimMiner >();
        
    miner->loadKey ( "test/keys/9090.mining-key.json" );
    miner->loadGenesisBlock ( "test/genesis" );
    miner->setMinerID ( "9090" );
    miner->affirmKey ();
    miner->affirmVisage ();
    
    Poco::Net::HTTPServer server ( new MinerAPIFactory ( miner ), Poco::Net::ServerSocket ( 9090 ), new Poco::Net::HTTPServerParams );
    server.start ();

    Poco::JSON::Object::Ptr json = httpGetJSON ( "http://127.0.0.1:9090/" );
    ASSERT_FALSE ( json.isNull ());
    
    ASSERT_TRUE ( json->getValue < string >( "type" ) == "VOL_MINING_NODE" );
    ASSERT_TRUE ( json->getValue < string >( "minerID" ) == "9090" );

    CryptoKeyPair keyFor9090;
    FromJSONSerializer::fromJSONFile ( keyFor9090, "test/keys/key9090.priv.json" );
    string sendVOLTransaction = ToJSONSerializer::toJSONString ( *makeTransaction (
        makeTransactionBody_SendVOL ( "9091", 100 ),
        "72e09129-098d-4c83-a759-8f35963738a5",
        keyFor9090,
        "9090"
    ));

    json = httpPutJSON ( "http://127.0.0.1:9090/accounts/9090/transactions/72e09129-098d-4c83-a759-8f35963738a5", sendVOLTransaction );
    ASSERT_FALSE ( json.isNull ());
    
    miner->step ( 0 );
    ASSERT_TRUE ( miner->getChainSize () == 2 );

    json = httpGetJSON ( "http://127.0.0.1:9090/accounts/9090" );
    ASSERT_FALSE ( json.isNull ());

    json = json->getObject ( "account" );
    ASSERT_FALSE ( json.isNull ());

    ASSERT_TRUE ( json->getValue < string >( "name" ) == "9090" );
    ASSERT_TRUE ( json->getValue < int >( "balance" ) == 999900 );
    ASSERT_TRUE ( json->getValue < int >( "nonce" ) == 1 );

    json = httpGetJSON ( "http://127.0.0.1:9090/accounts/9091" );
    ASSERT_FALSE ( json.isNull ());

    json = json->getObject ( "account" );

    ASSERT_FALSE ( json.isNull ());
    ASSERT_TRUE ( json->getValue < string >( "name" ) == "9091" );
    ASSERT_TRUE ( json->getValue < int >( "balance" ) == 1000100 );
    ASSERT_TRUE ( json->getValue < int >( "nonce" ) == 0 );

    server.stop ();
}

//----------------------------------------------------------------//
TEST ( WebMiner, consensus_api_get_headers ) {

    shared_ptr < SimMiner > miner = make_shared < SimMiner >();
        
    miner->loadKey ( "test/keys/9090.mining-key.json" );
    miner->loadGenesisBlock ( "test/genesis" );
    miner->setMinerID ( "9090" );
    miner->affirmKey ();
    miner->affirmVisage ();
    
    for ( size_t i = 0; i < ( ConsensusBlockHeaderListHandler::HEADER_BATCH_SIZE * 2 ); ++i ) {
        miner->extendChain ( Format::write ( "%llu", i ));
    }
    
    Poco::Net::HTTPServer server ( new MinerAPIFactory ( miner ), Poco::Net::ServerSocket ( 9090 ), new Poco::Net::HTTPServerParams );
    server.start ();

    {
        Poco::JSON::Object::Ptr json = httpGetJSON ( "http://127.0.0.1:9090/consensus/headers" );
        ASSERT_FALSE ( json.isNull ());
        ASSERT_TRUE ( json->optValue < string >( "headers", "" ) != "" );

        SerializableVector < SerializableSharedConstPtr < BlockHeader >> headers;
        FromJSONSerializer::fromJSONString ( headers, json->getValue < string >( "headers" ));

        ASSERT_TRUE ( headers.size () == ConsensusBlockHeaderListHandler::HEADER_BATCH_SIZE );

        size_t base = (( ConsensusBlockHeaderListHandler::HEADER_BATCH_SIZE * 2 ) + 1 ) - ConsensusBlockHeaderListHandler::HEADER_BATCH_SIZE;
        for ( size_t i = 0; i < ConsensusBlockHeaderListHandler::HEADER_BATCH_SIZE; ++i ) {
            shared_ptr < const BlockHeader > header = headers [ i ];
            shared_ptr < const Block > block = miner->getLedger ().getBlock ( base + i );
            ASSERT_TRUE ( header->equals ( *block ));
            ASSERT_TRUE ( header->getCharm () == block->getCharm ());
        }
    }
    
    {
        Poco::JSON::Object::Ptr json = httpGetJSON ( Format::write ( "http://127.0.0.1:9090/consensus/headers?height=%llu", ConsensusBlockHeaderListHandler::HEADER_BATCH_SIZE ));
        ASSERT_FALSE ( json.isNull ());
        ASSERT_TRUE ( json->optValue < string >( "headers", "" ) != "" );

        SerializableVector < SerializableSharedConstPtr < BlockHeader >> headers;
        FromJSONSerializer::fromJSONString ( headers, json->getValue < string >( "headers" ));

        ASSERT_TRUE ( headers.size () == ConsensusBlockHeaderListHandler::HEADER_BATCH_SIZE );

        size_t base = 1;
        for ( size_t i = 0; i < ConsensusBlockHeaderListHandler::HEADER_BATCH_SIZE; ++i ) {
            shared_ptr < const BlockHeader > header = headers [ i ];
            shared_ptr < const Block > block = miner->getLedger ().getBlock ( base + i );
            ASSERT_TRUE ( header->equals ( *block ));
            ASSERT_TRUE ( header->getCharm () == block->getCharm ());
        }
    }

    server.stop ();
}
