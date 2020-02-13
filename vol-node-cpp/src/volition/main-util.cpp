// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/CryptoKey.h>
#include <volition/CryptoKeyInfo.h>
#include <volition/TheContext.h>

//----------------------------------------------------------------//
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
        
        Volition::CryptoKey cryptoKey;

        if ( keyfile.size () && Poco::File ( keyfile ).exists ()) {
            fstream inStream;
            inStream.open ( keyfile, ios_base::in );
            Volition::FromJSONSerializer::fromJSON ( cryptoKey, inStream );
        }
        else {
            cryptoKey.elliptic ( Volition::CryptoKey::DEFAULT_EC_GROUP_NAME );
        }

        Volition::CryptoKeyInfo keyInfo ( cryptoKey, Volition::CryptoKeyInfo::PEM );
        Poco::Dynamic::Var var = Volition::ToJSONSerializer::toJSON ( keyInfo );
        Poco::JSON::Object::Ptr object = var.extract < Poco::JSON::Object::Ptr >();

        fstream jsonOutStream;
        jsonOutStream.open ( outfile, ios_base::out );
        object->stringify ( jsonOutStream, 4 );
        jsonOutStream.close ();
        
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
        
        Volition::CryptoKey keyPair;
        
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

//POCO_APP_MAIN ( SignBlockApp );

//----------------------------------------------------------------//
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
    
    if ( command == "sign-block" ) {
        return runApp ( argc, argv, new SignBlockApp );
    }

    return 0;
}