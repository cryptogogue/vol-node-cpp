// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_DEFAULTHANDLER_H
#define VOLITION_WEBMINERAPI_DEFAULTHANDLER_H

#include <volition/Block.h>
#include <volition/PayoutPolicy.h>
#include <volition/TransactionFeeSchedule.h>
#include <volition/Format.h>
#include <volition/AbstractMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/version.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// DefaultHandler
//================================================================//
class DefaultHandler :
    public AbstractMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus AbstractMinerAPIRequestHandler_handleRequest ( HTTP::Method method, shared_ptr < Miner > miner, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
        
        ScopedSharedMinerStatusLock minerStatus ( miner );
                
        jsonOut.set ( "type",                   "VOL_MINING_NODE" );
        jsonOut.set ( "minerID",                minerStatus.getMinerID ().c_str ());
        jsonOut.set ( "isMiner",                minerStatus.mIsMiner );
        jsonOut.set ( "started",                ( string )( SerializableTime ( minerStatus.getStartTime ())));
        jsonOut.set ( "genesis",                minerStatus.mGenesisHash );
        jsonOut.set ( "identity",               minerStatus.mIdentity );
        jsonOut.set ( "schemaVersion",          ToJSONSerializer::toJSON ( minerStatus.mSchemaVersion ));
        jsonOut.set ( "schemaHash",             minerStatus.mSchemaHash );
        jsonOut.set ( "build",                  Format::write ( "%s %s", VOLITION_BUILD_DATE_STR, VOLITION_GIT_TAG_STR ));
        jsonOut.set ( "commit",                 Format::write ( "%s", VOLITION_GIT_COMMIT_STR ));
        jsonOut.set ( "minGratuity",            minerStatus.mMinimumGratuity );
        jsonOut.set ( "reward",                 minerStatus.mReward );
        jsonOut.set ( "totalBlocks",            minerStatus.mTotalBlocks );
        jsonOut.set ( "minerBlocks",            minerStatus.mMinerBlockCount );
        jsonOut.set ( "feeSchedule",            ToJSONSerializer::toJSON ( minerStatus.mFeeSchedule ));
        jsonOut.set ( "monetaryPolicy",         ToJSONSerializer::toJSON ( minerStatus.mMonetaryPolicy ));
        jsonOut.set ( "payoutPolicy",           ToJSONSerializer::toJSON ( minerStatus.mPayoutPolicy ));

        jsonOut.set ( "rewardPool",             minerStatus.mRewardPool );
        jsonOut.set ( "prizePool",              minerStatus.mPrizePool );
        jsonOut.set ( "payoutPool",             minerStatus.mPayoutPool );
        jsonOut.set ( "vol",                    minerStatus.mVOL );

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
