// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/TheTransactionFactory.h>
#include <volition/RouteTable.h>
#include <volition/Singleton.h>
#include <volition/TheContext.h>
#include <volition/TheWebMiner.h>

//================================================================//
// DefaultHandler
//================================================================//
class DefaultHandler :
    public Volition::AbstractRequestHandler {
private:

    static int count;
    
public:

    //----------------------------------------------------------------//
    void AbstractRequestHandler_handleRequest ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response ) const override {

        response.setStatus ( Poco::Net::HTTPResponse::HTTP_OK );
        response.setContentType ( "text/html" );

        ostream& out = response.send ();
        out << "<h1>Hello world!</h1>"
        << "<p>Count: "     << ++count                  << "</p>"
        << "<p>Host: "      << request.getHost ()       << "</p>"
        << "<p>Method: "    << request.getMethod ()     << "</p>"
        << "<p>URI: "       << request.getURI ()        << "</p>";
        out.flush ();
    }
};

int DefaultHandler::count = 0;

//================================================================//
// FooHandler
//================================================================//
class FooHandler :
    public Volition::AbstractRequestHandler {
protected:

    //----------------------------------------------------------------//
    void AbstractRequestHandler_handleRequest ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response ) const override {
        response.setStatus ( Poco::Net::HTTPResponse::HTTP_OK );
        response.setContentType ( "text/html" );

        ostream& out = response.send ();
        out << "<h1>FOO HANDLER!</h1>";
        out.flush ();
    }
};

//================================================================//
// FooBarHandler
//================================================================//
class FooBarHandler :
    public Volition::AbstractRequestHandler {
protected:

    //----------------------------------------------------------------//
    void AbstractRequestHandler_handleRequest ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response ) const override {
        response.setStatus ( Poco::Net::HTTPResponse::HTTP_OK );
        response.setContentType ( "text/html" );

        ostream& out = response.send ();
        out << "<h1>FOOBAR HANDLER!</h1>";
        out.flush ();
    }
};

//================================================================//
// FooBarBazHandler
//================================================================//
class FooBarBazHandler :
    public Volition::AbstractRequestHandler {
protected:

    //----------------------------------------------------------------//
    void AbstractRequestHandler_handleRequest ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response ) const override {
        response.setStatus ( Poco::Net::HTTPResponse::HTTP_OK );
        response.setContentType ( "text/html" );

        ostream& out = response.send ();
        out << "<h1>" << match [ "baz" ] << "</h1>";
        out.flush ();
    }
};

//================================================================//
// TransactionHandler
//================================================================//
class TransactionHandler :
    public Volition::AbstractRequestHandler {
protected:

    //----------------------------------------------------------------//
    void AbstractRequestHandler_handleRequest ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response ) const override {
        
//        Poco::JSON::Object::Ptr object = AbstractRequestHandler::parseJSON ( request );
//
//        unique_ptr < Volition::AbstractTransaction > transaction ( Volition::TheTransactionFactory::get ().create ( *object ));
//        TheMiner::get ().pushTransaction ( transaction );
//
//        response.setStatus ( Poco::Net::HTTPResponse::HTTP_OK );
//        response.setContentType ( "application/json" );
//
//        ostream& out = response.send ();
//        out << "{\"foo\":\"bar\"}";
//        out.flush ();
    }
};

//================================================================//
// MyRequestHandlerFactory
//================================================================//
class MyRequestHandlerFactory :
    public Poco::Net::HTTPRequestHandlerFactory {
private:

    // thread local over mutex; trade a little memory for speed.
    // (though not sure we need a ThreadLocal here...)
    Poco::ThreadLocal < Volition::RouteTable > mRouteTable;

public:

    //----------------------------------------------------------------//
    void AffirmRouteTable () {
    
        if ( this->mRouteTable->size () > 0 ) return;
        
        this->mRouteTable->addEndpoint < FooHandler >           ( "/foo/?" );
        this->mRouteTable->addEndpoint < FooBarHandler >        ( "/foo/bar/?" );
        this->mRouteTable->addEndpoint < FooBarBazHandler >     ( "/foo/bar/:baz/?" );
        this->mRouteTable->addEndpoint < TransactionHandler >   ( "/transaction/?" );
        this->mRouteTable->setDefault < DefaultHandler >        ();
        
        // "/accounts/:accountName/?"           GET
        // "/accounts/:accountName/balance/?"   GET
        // "/accounts/:accountName/keys/?"      GET
        // "/blocks/?"                          GET
        // "/blocks/:blockID/?"                 GET
        // "/ledger/?"                          GET
        // "/transactions/?"                    POST
        // "/miners/?"                          GET
        // "/miners/:minerID/?"                 GET
    }

    //----------------------------------------------------------------//
    MyRequestHandlerFactory () {
    }

    //----------------------------------------------------------------//
    Poco::Net::HTTPRequestHandler* createRequestHandler ( const Poco::Net::HTTPServerRequest& request ) override {
        
        this->AffirmRouteTable ();
        return this->mRouteTable->match ( request.getURI ());
    }
};

//================================================================//
// ServerApp
//================================================================//
class ServerApp :
public Poco::Util::ServerApplication {
protected:

    //----------------------------------------------------------------//
    void defineOptions ( Poco::Util::OptionSet& options ) override {
        Application::defineOptions ( options );
        
        options.addOption (
            Poco::Util::Option ( "genesis", "g", "path to the genesis block" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "genesis" )
        );
        
        options.addOption (
            Poco::Util::Option ( "keyfile", "k", "path to key file" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "keyfile" )
        );
        
//        options.addOption (
//            Poco::Util::Option ( "name", "n", "mining node name" )
//                .required ( false )
//                .argument ( "value", true )
//                .binding ( "name" )
//        );
        
        options.addOption (
            Poco::Util::Option ( "port", "p", "set port to serve from" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "port" )
        );
        
        options.addOption (
            Poco::Util::Option ( "nodelist", "n", "path to nodelist file" )
                .required ( false )
                .argument ( "value" )
                .binding ( "nodelist" )
        );
        
        options.addOption (
            Poco::Util::Option ( "solo", "s", "operate in solo mode" )
                .required ( false )
                .argument ( "value" )
                .binding ( "solo" )
        );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string >& ) override {
        
        //this->printProperties ();
        
        Poco::Util::AbstractConfiguration& configuration = this->config ();
        
        string genesis      = configuration.getString ( "genesis" );
        string keyfile      = configuration.getString ( "keyfile" );
        int port            = configuration.getInt ( "port", 9090 );
        string nodelist     = configuration.getString ( "nodelist", "" );
        bool solo           = configuration.getBool ( "solo", false );
    
        LOG_F ( INFO, "SERVING YOU BLOCKCHAIN REALNESS ON PORT: %d\n", port );
    
        string minerID      = to_string ( port );
    
        Volition::TheContext::get ().setScoringMode ( Volition::TheContext::ScoringMode::INTEGER );
    
        Volition::TheWebMiner& theMiner = Volition::TheWebMiner::get ();
        
        if ( solo ) {
            theMiner.setLazy ( true );
            theMiner.setSolo ( true );
        }
        
        theMiner.loadKey ( keyfile );
        theMiner.loadGenesis ( genesis );
        theMiner.setMinerID ( minerID );
        theMiner.start ();

        this->serve ( port );
        this->waitForTerminationRequest ();

        theMiner.shutdown ();
        return Application::EXIT_OK;
    }
    
    //----------------------------------------------------------------//
    void printProperties ( const std::string& base = "" ) {
    
        Poco::Util::AbstractConfiguration::Keys keys;
        config ().keys ( base, keys );
    
        if ( keys.empty ()) {
            if ( config ().hasProperty ( base )) {
                std::string msg;
                msg.append ( base );
                msg.append ( " = " );
                msg.append ( config ().getString ( base ));
                //logger ().information ( msg );
                printf ( "%s\n", msg.c_str ());
            }
        }
        else {
            for ( Poco::Util::AbstractConfiguration::Keys::const_iterator it = keys.begin (); it != keys.end (); ++it ) {
                std::string fullKey = base;
                if ( !fullKey.empty ()) fullKey += '.';
                fullKey.append ( *it );
                printProperties ( fullKey );
            }
        }
    }
    
    //----------------------------------------------------------------//
    void serve ( int port ) {
    
        Poco::Net::HTTPServer server ( &Volition::TheWebMiner::get (), Poco::Net::ServerSocket ( port ), new Poco::Net::HTTPServerParams );
        server.start ();

        // nasty little hack. POCO considers the set breakpoint signal to be a termination event.
        // need to find out how to stop POCO from doing this. in the meantime, this hack.
        #ifdef _DEBUG
            Poco::Event dummy;
            dummy.wait ();
        #else
            this->waitForTerminationRequest ();  // wait for CTRL-C or kill
        #endif

        server.stop ();
    }
};

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

    // force line buffering even when running as a spawned process
    setvbuf ( stdout, NULL, _IOLBF, 0 );
    setvbuf ( stderr, NULL, _IOLBF, 0 );

    loguru::init ( argc, argv );
    loguru::g_preamble = false;
    LOG_F ( INFO, "Hello from main.cpp!" );

    ServerApp app;
    return app.run ( argc, argv );
}
