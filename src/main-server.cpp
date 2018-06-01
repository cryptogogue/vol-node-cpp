//
//  main.cpp
//  consensus
//
//  Created by Patrick Meehan on 4/27/18.
//  Copyright © 2018 Patrick Meehan. All rights reserved.
//

#include "analysis.h"
#include "context.h"
#include "cohort.h"
#include "scenario.h"

#include "Block.h"
#include "TheMiner.h"
#include "RouteTable.h"

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

        cout << endl
        << "Response sent for count=" << count
        << " and URI=" << request.getURI () << endl;
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
        this->mRouteTable->setDefault < DefaultHandler >        ();
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
            Poco::Util::Option ( "keyfile", "k", "path to key file" )
                .required ( true )
                .argument ( "value", true )
                .binding ( "keyfile" )
        );
        
        options.addOption (
            Poco::Util::Option ( "port", "p", "set port to serve from" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "port" )
        );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string >& ) override {
    
        this->printProperties ();
    
        Poco::Util::AbstractConfiguration& configuration = this->config ();
        
        string keyfile  = configuration.getString ( "keyfile" );
        int port        = configuration.getInt ( "port", 8888 );
    
        printf ( "SERVING YOU BLOCKCHAIN REALNESS ON PORT: %d\n", port );
    
        Volition::TheMiner::get ().load ( keyfile );
    
        Volition::Block block;
        Poco::DigestEngine::Digest signature = Volition::TheMiner::get ().sign ( block );
        string sigString = Poco::DigestEngine::digestToHex ( signature );
        printf ( "SIG: %s\n", sigString.c_str ());
        
        //this->serve ( port );
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
                logger ().information ( msg );
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
    
        Poco::Net::HTTPServer server ( new MyRequestHandlerFactory, Poco::Net::ServerSocket ( port ), new Poco::Net::HTTPServerParams );
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
    ServerApp app;
    return app.run ( argc, argv );
}
