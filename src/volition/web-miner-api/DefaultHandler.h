// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_DEFAULTHANDLER_H
#define VOLITION_WEBMINERAPI_DEFAULTHANDLER_H

#include <volition/Block.h>
#include <volition/BlockingMinerAPIRequestHandler.h>
#include <volition/FeeDistributionTable.h>
#include <volition/FeeSchedule.h>
#include <volition/Format.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/version.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// DefaultHandler
//================================================================//
class DefaultHandler :
    public BlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus BlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, Ledger& ledger, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
                
        SchemaVersion schemaVersion = ledger.getSchemaVersion ();
        
        jsonOut.set ( "type",           "VOL_MINING_NODE" );
        jsonOut.set ( "minerID",        this->mWebMiner->getMinerID ().c_str ());
        jsonOut.set ( "started",        ( string )( SerializableTime ( this->mWebMiner->getStartTime ())));
        jsonOut.set ( "genesis",        ledger.getGenesisHash ());
        jsonOut.set ( "identity",       ledger.getIdentity ());
        jsonOut.set ( "schemaVersion",  ToJSONSerializer::toJSON ( schemaVersion ));
        jsonOut.set ( "schemaHash",     ledger.getSchemaHash ());
        jsonOut.set ( "build",          Format::write ( "%s %s", VOLITION_BUILD_DATE_STR, VOLITION_GIT_TAG_STR ));
        jsonOut.set ( "commit",         Format::write ( "%s", VOLITION_GIT_COMMIT_STR ));
        jsonOut.set ( "vol",            ledger.countVOL ());
        jsonOut.set ( "pool",           ledger.getFeeDistributionPool ());
        jsonOut.set ( "minGratuity",    this->mWebMiner->getMinimumGratuity ());
        jsonOut.set ( "reward",         this->mWebMiner->getReward ());
        jsonOut.set ( "totalBlocks",    ledger.countBlocks ());

        FeeSchedule feeSchedule = ledger.getFeeSchedule ();
        jsonOut.set ( "feeSchedule", ToJSONSerializer::toJSON ( feeSchedule ));

        FeeDistributionTable feeDistributionTable = ledger.getFeeDistributionTable ();
        jsonOut.set ( "feeDistributionTable", ToJSONSerializer::toJSON ( feeDistributionTable ));

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
