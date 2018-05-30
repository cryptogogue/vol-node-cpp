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

#include "VLRouteTable.h"

//================================================================//
// DefaultHandler
//================================================================//
class DefaultHandler :
    public VLAbstractRequestHandler {
private:

    static int count;
    
public:

    //----------------------------------------------------------------//
    void VLAbstractRequestHandler_HandleRequest ( HTTPServerRequest& request, HTTPServerResponse& response ) const override {
        response.setStatus ( HTTPResponse::HTTP_OK );
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
    public VLAbstractRequestHandlerWithMatch {
protected:

    //----------------------------------------------------------------//
    void VLAbstractRequestHandlerWithMatch_HandleRequest ( const PathMatch& match, HTTPServerRequest &request, HTTPServerResponse &response ) const override {
        response.setStatus ( HTTPResponse::HTTP_OK );
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
    public VLAbstractRequestHandlerWithMatch {
protected:

    //----------------------------------------------------------------//
    void VLAbstractRequestHandlerWithMatch_HandleRequest ( const PathMatch& match, HTTPServerRequest &request, HTTPServerResponse &response ) const override {
        response.setStatus ( HTTPResponse::HTTP_OK );
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
    public VLAbstractRequestHandlerWithMatch {
protected:

    //----------------------------------------------------------------//
    void VLAbstractRequestHandlerWithMatch_HandleRequest ( const PathMatch& match, HTTPServerRequest &request, HTTPServerResponse &response ) const override {
        response.setStatus ( HTTPResponse::HTTP_OK );
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
    public HTTPRequestHandlerFactory {
private:

    // thread local over mutex; trade a little memory for speed.
    Poco::ThreadLocal < VLRouteTable > mRouteTable;

public:

    //----------------------------------------------------------------//
    void AffirmRouteTable () {
    
        if ( this->mRouteTable->Size () > 0 ) return;
        
        this->mRouteTable->AddEndpoint < FooHandler >           ( "/foo/?" );
        this->mRouteTable->AddEndpoint < FooBarHandler >        ( "/foo/bar/?" );
        this->mRouteTable->AddEndpoint < FooBarBazHandler >     ( "/foo/bar/:baz/?" );
        this->mRouteTable->SetDefault < DefaultHandler >        ();
    }

    //----------------------------------------------------------------//
    MyRequestHandlerFactory () {
    }

    //----------------------------------------------------------------//
    HTTPRequestHandler* createRequestHandler ( const HTTPServerRequest& request ) override {
        
        this->AffirmRouteTable ();
        return this->mRouteTable->Match ( request.getURI ());
    }
};

//================================================================//
// ServerApp
//================================================================//
class ServerApp :
public ServerApplication {
protected:

    //----------------------------------------------------------------//
    int main ( const vector < string >& ) override {
    
        HTTPServer server ( new MyRequestHandlerFactory, ServerSocket ( 9090 ), new HTTPServerParams );

        server.start ();
        printf ( "Server started\n" );

        // nasty little hack. POCO considers the set breakpoint signal to be a termination event.
        // need to find out how to stop POCO from doing this. in the meantime, this hack.
        #ifdef _DEBUG
            Poco::Event dummy;
            dummy.wait ();
        #else
            waitForTerminationRequest ();  // wait for CTRL-C or kill
        #endif

        printf ( "Shutting down...\n" );
        server.stop ();

        return Application::EXIT_OK;
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