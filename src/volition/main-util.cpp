// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/CryptoKey.h>
#include <volition/Miner.h>
#include <volition/Transaction.h>
#include <volition/Transactions.h>

using namespace Volition;

//----------------------------------------------------------------//
void defineKeyFileOptions ( Poco::Util::OptionSet& options, bool required );
void defineKeyFileOptions ( Poco::Util::OptionSet& options, bool required ) {

    options.addOption (
        Poco::Util::Option ( "keyfile", "k", "path to key file" )
            .required ( required )
            .argument ( "value", true )
            .binding ( "keyfile" )
    );

    options.addOption (
        Poco::Util::Option ( "password", "p", "key file password" )
            .required ( false )
            .argument ( "value", true )
            .binding ( "password" )
    );
}

//================================================================//
// DumpKeyApp
//================================================================//
class DumpKeyApp :
    public Poco::Util::Application {
public:

    //----------------------------------------------------------------//
    void defineOptions ( Poco::Util::OptionSet& options ) override {
        Application::defineOptions ( options );
        defineKeyFileOptions ( options, false );
        
        options.addOption (
            Poco::Util::Option ( "outfile", "o", "output file" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "outfile" )
        );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
        UNUSED ( args );
        
        Poco::Util::AbstractConfiguration& configuration = this->config ();
    
        string keyfile      = configuration.getString ( "keyfile", "" );
        string password     = configuration.getString ( "password", "" );
        string outfile      = configuration.getString ( "outfile" );
        
        Volition::CryptoKeyPair cryptoKey;

        if ( keyfile.size () && Poco::File ( keyfile ).exists ()) {
            fstream inStream;
            inStream.open ( keyfile, ios_base::in );
            FromJSONSerializer::fromJSON ( cryptoKey, inStream );
        }
        else {
            cryptoKey.rsa ( Volition::CryptoKeyPair::RSA_4096 );
        }
        
        ToJSONSerializer::toJSONFile ( cryptoKey, outfile );
        
        return EXIT_OK;
    }
};

//================================================================//
// DumpMinerApp
//================================================================//
class DumpMinerApp :
    public Poco::Util::Application {
public:

    //----------------------------------------------------------------//
    void defineOptions ( Poco::Util::OptionSet& options ) override {
        Application::defineOptions ( options );
        defineKeyFileOptions ( options, false );
        
        options.addOption (
            Poco::Util::Option ( "name", "n", "miner name" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "name" )
        );
        
        options.addOption (
            Poco::Util::Option ( "motto", "m", "miner motto" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "motto" )
        );
        
        options.addOption (
            Poco::Util::Option ( "url", "u", "miner url" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "url" )
        );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
        UNUSED ( args );
        
        Poco::Util::AbstractConfiguration& configuration = this->config ();
    
        string keyfile      = configuration.getString ( "keyfile", "" );
        string password     = configuration.getString ( "password", "" );
        string name         = configuration.getString ( "name" );
        string motto        = configuration.getString ( "motto", "" );
        string url          = configuration.getString ( "url" );
        
        shared_ptr < Volition::Miner > miner = make_shared < Volition::Miner >();
        
        CryptoKeyPair cryptoKey;

        if ( keyfile.size () && Poco::File ( keyfile ).exists ()) {
            fstream inStream;
            inStream.open ( keyfile, ios_base::in );
            Volition::FromJSONSerializer::fromJSON ( cryptoKey, inStream );
        }
        else {
            cryptoKey.rsa ( Volition::CryptoKeyPair::RSA_4096 );
        }

        miner->setKeyPair ( cryptoKey );
        miner->setMinerID ( name );
        miner->setMotto ( motto );
        miner->affirmKey ();
        miner->affirmVisage ();

        Transactions::GenesisAccount genesisAccount;
        
        shared_ptr < const MinerInfo > minerInfo = make_shared < MinerInfo >(
            url,
            miner->getKeyPair ().getPublicKey (),
            miner->getMotto (),
            miner->getVisage ()
        );
        
        genesisAccount.mName            = miner->getMinerID ();
        genesisAccount.mKey             = miner->getKeyPair ().getPublicKey ();
        genesisAccount.mGrant           = 0;
        genesisAccount.mMinerInfo       = minerInfo;

        ToJSONSerializer::toJSONFile ( genesisAccount, Format::write ( "%s.account.json", name.c_str ()));
        ToJSONSerializer::toJSONFile ( cryptoKey, Format::write ( "%s.keypair.json", name.c_str ()));
        
        return EXIT_OK;
    }
};

//================================================================//
// GenesisFromLedgerApp
//================================================================//
class GenesisFromLedgerApp :
    public Poco::Util::Application {
public:

    //----------------------------------------------------------------//
    void defineOptions ( Poco::Util::OptionSet& options ) override {
        Application::defineOptions ( options );
        defineKeyFileOptions ( options, false );
        
        options.addOption (
            Poco::Util::Option ( "in", "i", "infile" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "infile" )
        );
        
        options.addOption (
            Poco::Util::Option ( "out", "o", "outfile" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "outfile" )
        );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
        UNUSED ( args );
        
        Poco::Util::AbstractConfiguration& configuration = this->config ();
    
        string infile       = configuration.getString ( "infile", "" );
        string outfile      = configuration.getString ( "outfile", "" );
        
        shared_ptr < Transactions::LoadLedger > loadLedger = make_shared < Transactions::LoadLedger >();
        FromJSONSerializer::fromJSONFile ( *loadLedger, infile );
        
        shared_ptr < Transaction > transaction = make_shared < Transaction >();
        transaction->setBody ( loadLedger );
        
        shared_ptr < Block > block = make_shared < Block >();
        block->pushTransaction ( transaction );

        ToJSONSerializer::toJSONFile ( *block, outfile );
        
        return EXIT_OK;
    }
};

//================================================================//
// SignBlockApp
//================================================================//
class SignBlockApp :
    public Poco::Util::Application {
public:

    //----------------------------------------------------------------//
    void defineOptions ( Poco::Util::OptionSet& options ) override {
        Application::defineOptions ( options );
        defineKeyFileOptions ( options, true );
        
        options.addOption (
        Poco::Util::Option ( "inpath", "i", "path to input file" )
            .required ( true )
            .argument ( "value", true )
            .binding ( "inpath" )
        );
        
        options.addOption (
            Poco::Util::Option ( "outpath", "o", "path to output file" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "outpath" )
        );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
        UNUSED ( args );
        
        Poco::Util::AbstractConfiguration& configuration = this->config ();
    
        string keyfile      = configuration.getString ( "keyfile" );
        string password     = configuration.getString ( "password", "" ); // TODO: password
        string inpath       = configuration.getString ( "inpath" );
        string outpath      = configuration.getString ( "outpath", "" );
        
        Volition::CryptoKeyPair keyPair;
        
        fstream keyStream;
        keyStream.open ( keyfile, ios_base::in );
        Volition::FromJSONSerializer::fromJSON ( keyPair, keyStream );

        fstream inStream;
        inStream.open ( inpath, ios_base::in );

        Volition::Block block;
        Volition::FromJSONSerializer::fromJSON ( block, inStream );
        block.sign ( keyPair );

        fstream outStream;
        outStream.open ( outpath, ios_base::out );
        Volition::ToJSONSerializer::toJSON ( block, outStream );
        
        return EXIT_OK;
    }
};

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int runApp ( int argc, char** argv, Poco::Util::Application* app );
int runApp ( int argc, char** argv, Poco::Util::Application* app ) {

    Poco::AutoPtr < Poco::Util::Application > pApp = app;
    try {
        pApp->init ( argc, argv );
    }
    catch ( Poco::Exception& exc ) {
        pApp->logger ().log ( exc );
        return Poco::Util::Application::EXIT_CONFIG;
    }
    return pApp->run();
}

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

    Lognosis::setFilter ( PDM_FILTER_ROOT, Lognosis::OFF );

    assert ( argc >= 2 );
    string command = argv [ 1 ];

    if ( command == "dump-key" ) {
        return runApp ( argc, argv, new DumpKeyApp );
    }
    
    if ( command == "dump-miner" ) {
        return runApp ( argc, argv, new DumpMinerApp );
    }
    
    if ( command == "genesis-from-ledger" ) {
        return runApp ( argc, argv, new GenesisFromLedgerApp );
    }
    
    if ( command == "sign-block" ) {
        return runApp ( argc, argv, new SignBlockApp );
    }

    return 0;
}
