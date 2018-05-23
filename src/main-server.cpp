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

//================================================================//
// MyRequestHandler
//================================================================//
class MyRequestHandler :
	public HTTPRequestHandler {
private:

	static int count;
	
public:

	//----------------------------------------------------------------//
	void handleRequest ( HTTPServerRequest &req, HTTPServerResponse &resp ) override {
		resp.setStatus ( HTTPResponse::HTTP_OK );
		resp.setContentType ( "text/html" );

		ostream& out = resp.send ();
		out << "<h1>Hello world!</h1>"
		<< "<p>Count: "		<< ++count				<< "</p>"
		<< "<p>Host: "		<< req.getHost ()		<< "</p>"
		<< "<p>Method: "	<< req.getMethod ()		<< "</p>"
		<< "<p>URI: "		<< req.getURI ()		<< "</p>";
		out.flush ();

		cout << endl
		<< "Response sent for count=" << count
		<< " and URI=" << req.getURI () << endl;
	}
};

int MyRequestHandler::count = 0;

//================================================================//
// MyRequestHandler
//================================================================//
class MyRequestHandlerFactory :
	public HTTPRequestHandlerFactory {
public:

	//----------------------------------------------------------------//
	HTTPRequestHandler* createRequestHandler ( const HTTPServerRequest & ) override {
		return new MyRequestHandler;
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

		waitForTerminationRequest ();  // wait for CTRL-C or kill

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
