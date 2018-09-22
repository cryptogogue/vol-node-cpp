// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef COMMON_H
#define COMMON_H

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
#include <Poco/SingletonHolder.h>
#include <Poco/Task.h>
#include <Poco/TaskManager.h>
#include <Poco/TaskNotification.h>
#include <Poco/ThreadLocal.h>
#include <Poco/URI.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/ServerApplication.h>

#include <Routing/Exceptions.h>
#include <Routing/Router.h>

#include <loguru.hpp>

using namespace std;

//----------------------------------------------------------------//
typedef unsigned int            uint;

typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;

typedef signed char             s8;
typedef signed short            s16;
typedef signed int              s32;
typedef signed long long        s64;

#endif
