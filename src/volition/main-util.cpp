// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheContext.h>

//----------------------------------------------------------------//
void defineKeyFileOptions ( Poco::Util::OptionSet& options ) {

    options.addOption (
        Poco::Util::Option ( "keyfile", "k", "path to key file" )
            .required ( true )
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
        defineKeyFileOptions ( options );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
        
        Poco::Util::AbstractConfiguration& configuration = this->config ();
    
        string keyfile      = configuration.getString ( "keyfile" );
        string password     = configuration.getString ( "password", "" );
        
        unique_ptr < Poco::Crypto::ECKey > keyPair;
        
        if ( Poco::File ( keyfile ).exists ()) {
            keyPair = make_unique < Poco::Crypto::ECKey >( "", keyfile, password );
        }
        else {
            keyPair = make_unique < Poco::Crypto::ECKey >( Volition::TheContext::EC_CURVE );
        }
        
        // dump to pem
        
        fstream pubKeyOutStream;
        pubKeyOutStream.open ( keyfile + ".pub", ios_base::out );
        
        fstream privKeyOutStream;
        privKeyOutStream.open ( keyfile + ".priv", ios_base::out );
        
        keyPair->save ( &pubKeyOutStream, &privKeyOutStream, password );
        
        pubKeyOutStream.close ();
        privKeyOutStream.close ();
        
        // dump to json
        
        Poco::JSON::Object::Ptr object = new Poco::JSON::Object ();
        
        stringstream pubKeyStringStream;
        stringstream privKeyStringStream;
        
        keyPair->save ( &pubKeyStringStream, &privKeyStringStream, password );
        
        object->set ( "publicKey", pubKeyStringStream.str ().c_str ());
        object->set ( "privateKey", privKeyStringStream.str ().c_str ());
        
        fstream jsonOutStream;
        jsonOutStream.open ( keyfile + ".json", ios_base::out );
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
        defineKeyFileOptions ( options );
        
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
        
        Poco::Util::AbstractConfiguration& configuration = this->config ();
    
        string keyfile      = configuration.getString ( "keyfile" );
        string password     = configuration.getString ( "password", "" );
        string inpath       = configuration.getString ( "inpath" );
        string outpath      = configuration.getString ( "outpath", "" );
        
        Poco::Crypto::ECKey keyPair = Poco::Crypto::ECKey ( "", keyfile, password );
        
        fstream inStream;
        inStream.open ( inpath, ios_base::in );
        
        Volition::Block block;
        block.fromJSON ( inStream );
        block.sign ( keyPair );
        
        fstream outStream;
        outStream.open ( outpath, ios_base::out );
        block.toJSON ( outStream );
        
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
    catch ( Poco::Exception& exc )
    {
        pApp->logger ().log ( exc );
        return Poco::Util::Application::EXIT_CONFIG;
    }
    return pApp->run();
}

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

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
