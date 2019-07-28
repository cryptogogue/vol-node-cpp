// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef COMMON_H
#define COMMON_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include <assert.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <inttypes.h>
#include <iostream>
#include <limits>
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
#include <Poco/Net/SecureServerSocket.h>
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

#include <Routing/Exceptions.h>
#include <Routing/Router.h>

#include <padamose/VersionedCollectionIterator.h>
#include <padamose/VersionedMap.h>
#include <padamose/VersionedList.h>
#include <padamose/VersionedStore.h>
#include <padamose/VersionedStoreIterator.h>
#include <padamose/VersionedValueIterator.h>

#include <lua.hpp>

#include <lognosis/Logger.h>

#pragma clang diagnostic pop

using namespace std;
using namespace Padamose;

//----------------------------------------------------------------//
typedef unsigned int            uint;

typedef Poco::UInt8             u8;
typedef Poco::UInt16            u16;
typedef Poco::UInt32            u32;
typedef Poco::UInt64            u64;

typedef Poco::Int8              s8;
typedef Poco::Int16             s16;
typedef Poco::Int32             s32;
typedef Poco::Int64             s64;

#define VOL_FILTER_ROOT "volition"

#if !defined(UNUSED)
    #define UNUSED(x) ((void)(x))
#endif

#endif
