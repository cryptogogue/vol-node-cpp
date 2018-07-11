// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THEWEBMINERAPI_H
#define VOLITION_THEWEBMINERAPI_H

#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace TheWebMinerAPI {

//================================================================//
// AccountDetailsHandler
//================================================================//
class AccountDetailsHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const override {
    
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

//================================================================//
// AccountKeysHandler
//================================================================//
class AccountKeysHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const override {
    
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

//================================================================//
// BlockDetailsHandler
//================================================================//
class BlockDetailsHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const override {
    
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

//================================================================//
// BlockListHandler
//================================================================//
class BlockListHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const override {
        
        const Chain* chain = TheWebMiner::get ().getChain ();
        if ( chain ) {
            jsonOut = chain->toJSON ();
        }
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

//================================================================//
// DefaultHandler
//================================================================//
class DefaultHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_ALL )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const override {
    
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

//================================================================//
// MinerDetailsHandler
//================================================================//
class MinerDetailsHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const override {
    
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

//================================================================//
// MinerListHandler
//================================================================//
class MinerListHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const override {
    
         return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

//================================================================//
// TransactionHandler
//================================================================//
class TransactionHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_POST )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Routing::PathMatch& match, const Poco::JSON::Object::Ptr jsonIn, Poco::JSON::Object::Ptr& jsonOut ) const override {

        unique_ptr < AbstractTransaction > transaction ( TheTransactionFactory::get ().create ( *jsonIn ));
        TheWebMiner::get ().pushTransaction ( transaction );
        
        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
