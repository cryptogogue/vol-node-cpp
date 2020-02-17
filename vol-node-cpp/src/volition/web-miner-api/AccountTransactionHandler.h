// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_ACCOUNTTRANSACTIONSHANDLER_H
#define VOLITION_WEBMINERAPI_ACCOUNTTRANSACTIONSHANDLER_H

#include <volition/Block.h>
#include <volition/AbstractAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// AccountTransactionHandler
//================================================================//
class AccountTransactionHandler :
    public AbstractAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET_PUT )

    //----------------------------------------------------------------//
    HTTPStatus AbstractAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
    
        string accountName  = this->getMatchString ( "accountName" );
        u64 nonce           = this->getMatchU64 ( "nonce" );
    
        ScopedWebMinerLock scopedLock ( TheWebMiner::get ());
        WebMiner& miner = scopedLock.getWebMiner ();
    
        switch ( method ) {
    
            case HTTP::GET: {

                if ( miner.isRejected ( accountName )) {
                    jsonOut.set ( "status", "REJECTED" );
                    return Poco::Net::HTTPResponse::HTTP_OK;
                }

                const Chain& chain = *miner.getBestBranch ();
                string note = chain.getTransactionNote ( accountName, nonce );

                if ( note.size () > 0 ) {
                    jsonOut.set ( "status", "ACCEPTED" );
                    jsonOut.set ( "note", note );
                    return Poco::Net::HTTPResponse::HTTP_OK;
                }

                note = miner.getTransactionNote ( accountName, nonce );
                if ( note.size () > 0 ) {
                    jsonOut.set ( "status", "PENDING" );
                    jsonOut.set ( "note", note );
                    return Poco::Net::HTTPResponse::HTTP_OK;
                }
                return Poco::Net::HTTPResponse::HTTP_OK;
            }
            
            case HTTP::PUT: {

                SerializableUniquePtr < Transaction > transaction;
                FromJSONSerializer::fromJSON ( transaction, jsonIn );

                if ( transaction && transaction->checkMaker ( accountName, nonce )) {
                    scopedLock.getWebMiner ().pushTransaction ( move ( transaction ));
                    return Poco::Net::HTTPResponse::HTTP_OK;
                }
                return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
            }
            
            default:
                break;
        }
        return Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
