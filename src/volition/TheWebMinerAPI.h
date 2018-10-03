// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THEWEBMINERAPI_H
#define VOLITION_THEWEBMINERAPI_H

#include <volition/Block.h>
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
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        string accountName = this->getMatchString ( "accountName" );
        const State& state = TheWebMiner::get ().getState ();
        
        VersionedValue < Account > account = state.getAccount ( accountName );
        if ( account ) {
            Poco::JSON::Object::Ptr accountJSON = new Poco::JSON::Object ();
            accountJSON->set ( "accountName", accountName.c_str ());
            accountJSON->set ( "balance", ( int )account->getBalance ());
            
            jsonOut.set ( "account", accountJSON );
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
    }
};

//================================================================//
// AccountKeyListHandler
//================================================================//
class AccountKeyListHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        string accountName = this->getMatchString ( "accountName" );
        const State& state = TheWebMiner::get ().getState ();
        
        VersionedValue < Account > account = state.getAccount ( accountName );
        if ( account ) {
    
            map < string, CryptoKey > keys;
            account->getKeys ( keys );
            
            Poco::JSON::Object::Ptr keysJSON = new Poco::JSON::Object ();
    
            map < string, CryptoKey >::iterator keyIt = keys.begin ();
            for ( ; keyIt != keys.end (); ++keyIt ) {
                Poco::JSON::Object::Ptr keyJSON = ToJSONSerializer::toJSON ( keyIt->second );
                keysJSON->set ( keyIt->first, keyJSON );
            }
            
            jsonOut.set ( "accountKeys", keysJSON );
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
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
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {

//        try {
//            u64 height = this->getMatchU64 ( "blockID" );
//
//            Chain* chain = TheWebMiner::get ().getChain ();
//            if ( chain ) {
//                Block* block = chain->findBlock ( height );
//                if ( block ) {
//                    jsonOut.set ( "block", ToJSONSerializer::toJSON ( *block ));
//                    return Poco::Net::HTTPResponse::HTTP_OK;
//                }
//            }
//        }
//        catch ( ... ) {
//            return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
//        }
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
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {

        Chain* chain = TheWebMiner::get ().getChain ();
        if ( chain ) {
            jsonOut.set ( "blocks", ToJSONSerializer::toJSON ( *chain ));
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

    SUPPORTED_HTTP_METHODS ( HTTP_GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        
        const TheWebMiner& theMiner = TheWebMiner::get ();
        
        jsonOut.set ( "minerID", theMiner.getMinerID ().c_str ());

        return Poco::Net::HTTPResponse::HTTP_OK;
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
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        const TheWebMiner& theMiner                     = TheWebMiner::get ();
        const State& state                              = theMiner.getState ();
        const map < string, MinerInfo >& minerInfo      = state.getMiners ();
        
        Poco::JSON::Object::Ptr minersJSON = new Poco::JSON::Object ();
        
        map < string, MinerInfo >::const_iterator minerInfoIt = minerInfo.cbegin ();
        for ( unsigned int i = 0; minerInfoIt != minerInfo.cend (); ++minerInfoIt, ++i ) {
            const MinerInfo& minerInfo = minerInfoIt->second;
            
            Poco::JSON::Object::Ptr minerInfoJSON = new Poco::JSON::Object ();
            minerInfoJSON->set ( "url",         minerInfo.getURL ().c_str ());
            minersJSON->set ( minerInfo.getMinerID (), minerInfoJSON );
        }
        
        jsonOut.set ( "miners", minersJSON );
        return Poco::Net::HTTPResponse::HTTP_OK;
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
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( int method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {

        JSONSerializableTypeInfo typeInfo ( jsonIn );
        TransactionFactory factory;
        unique_ptr < AbstractTransaction > transaction = factory.make ( typeInfo );
        if ( transaction ) {
            FromJSONSerializer::fromJSON ( *transaction, jsonIn );
            TheWebMiner::get ().pushTransaction ( move ( transaction ));
            return Poco::Net::HTTPResponse::HTTP_OK;
        }
        return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
