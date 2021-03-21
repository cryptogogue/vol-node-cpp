// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_WEBMINERAPI_DEFAULTHANDLER_H
#define VOLITION_WEBMINERAPI_DEFAULTHANDLER_H

#include <volition/Block.h>
#include <volition/PayoutPolicy.h>
#include <volition/TransactionFeeSchedule.h>
#include <volition/Format.h>
#include <volition/NonBlockingMinerAPIRequestHandler.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/version.h>

namespace Volition {
namespace WebMinerAPI {

//================================================================//
// DefaultHandler
//================================================================//
class DefaultHandler :
    public NonBlockingMinerAPIRequestHandler {
public:

    SUPPORTED_HTTP_METHODS ( HTTP::GET )

    //----------------------------------------------------------------//
    HTTPStatus NonBlockingMinerAPIRequestHandler_handleRequest ( HTTP::Method method, const Poco::JSON::Object& jsonIn, Poco::JSON::Object& jsonOut ) const override {
        UNUSED ( method );
        UNUSED ( jsonIn );
                
        const MinerSnapshot& snapshot   = this->mSnapshot;
        const MinerStatus& status       = this->mStatus;
                
        jsonOut.set ( "type",                   "VOL_MINING_NODE" );
        jsonOut.set ( "minerID",                snapshot.getMinerID ().c_str ());
        jsonOut.set ( "started",                ( string )( SerializableTime ( snapshot.getStartTime ())));
        jsonOut.set ( "genesis",                status.mGenesisHash );
        jsonOut.set ( "identity",               status.mIdentity );
        jsonOut.set ( "schemaVersion",          ToJSONSerializer::toJSON ( status.mSchemaVersion ));
        jsonOut.set ( "schemaHash",             status.mSchemaHash );
        jsonOut.set ( "build",                  Format::write ( "%s %s", VOLITION_BUILD_DATE_STR, VOLITION_GIT_TAG_STR ));
        jsonOut.set ( "commit",                 Format::write ( "%s", VOLITION_GIT_COMMIT_STR ));
        jsonOut.set ( "minGratuity",            status.mMinimumGratuity );
        jsonOut.set ( "reward",                 status.mReward );
        jsonOut.set ( "totalBlocks",            status.mTotalBlocks );
        jsonOut.set ( "minerBlocks",            status.mMinerBlockCount );
        jsonOut.set ( "feeSchedule",            ToJSONSerializer::toJSON ( status.mFeeSchedule ));
        jsonOut.set ( "monetaryPolicy",         ToJSONSerializer::toJSON ( status.mMonetaryPolicy ));
        jsonOut.set ( "payoutPolicy",           ToJSONSerializer::toJSON ( status.mPayoutPolicy ));

        jsonOut.set ( "rewardPool",             status.mRewardPool );
        jsonOut.set ( "prizePool",              status.mPrizePool );
        jsonOut.set ( "payoutPool",             status.mPayoutPool );
        jsonOut.set ( "vol",                    status.mVOL );

        return Poco::Net::HTTPResponse::HTTP_OK;
    }
};

} // namespace TheWebMinerAPI
} // namespace Volition
#endif
