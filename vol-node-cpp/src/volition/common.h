// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef COMMON_H
#define COMMON_H

#if __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wsign-compare"
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#if __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <assert.h>
#include <cmath>
#include <csignal>
#include <cstring>
#include <dirent.h>
#include <fstream>
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
#include <sys/types.h>
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
#include <Poco/Process.h>
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

#if __clang__
    #pragma clang diagnostic pop
#endif

#if __GNUC__
    #pragma GCC diagnostic pop
#endif

using namespace std;
using namespace Padamose;

//----------------------------------------------------------------//
typedef unsigned int            uint;

typedef uint8_t                 u8;
typedef uint16_t                u16;
typedef uint32_t                u32;
typedef uint64_t                u64;

typedef int8_t                  s8;
typedef int16_t                 s16;
typedef int32_t                 s32;
typedef int64_t                 s64;

#define VOL_FILTER_ROOT "volition"

#if !defined(UNUSED)
    #define UNUSED(x) ((void)(x))
#endif

#endif
