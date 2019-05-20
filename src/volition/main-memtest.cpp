// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <assert.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <inttypes.h>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <typeinfo>
#include <vector>

#include <Poco/Activity.h>
#include <Poco/Crypto/DigestEngine.h>
#include <Poco/Crypto/ECKey.h>
#include <Poco/Crypto/ECKeyImpl.h>
#include <Poco/Crypto/ECDSADigestEngine.h>
#include <Poco/DigestEngine.h>
#include <Poco/DigestStream.h>
#include <Poco/File.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/MD5Engine.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Observer.h>
#include <Poco/ScopedLock.h>
#include <Poco/SingletonHolder.h>
#include <Poco/Task.h>
#include <Poco/TaskManager.h>
#include <Poco/TaskNotification.h>
#include <Poco/ThreadLocal.h>
#include <Poco/URI.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/ServerApplication.h>

#include "JSONObject.h"

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

    {
        printf ( "THIS IS MY VOICE ON TEE VOY!\n" );
        printf ( "SIZEOF MOCO JSON OBJ: %d\n", ( int )sizeof ( Moco::JSON::Object ));
        printf ( "SIZEOF POCO JSON OBJ: %d\n", ( int )sizeof ( Poco::JSON::Object ));
        Poco::JSON::Object* object = new Poco::JSON::Object ();
        delete object;
    }
    printf ( "DONE!\n" );
}
