// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SIMTRANSACTION_H
#define VOLITION_SIMTRANSACTION_H

#include <volition/common.h>
#include <volition/CryptoKey.h>
#include <volition/Miner.h>
#include <volition/Signature.h>
#include <volition/TransactionEnvelope.h>
#include <volition/Transactions.h>

namespace Volition {
namespace Simulation {

//================================================================//
// SimTransaction
//================================================================//
class SimTransaction {
public:

    //----------------------------------------------------------------//
    static shared_ptr < AbstractTransaction > makeBody_RegisterMiner ( const Miner& miner, string url ) {

        return makeBody_RegisterMiner (
            miner.getMinerID (),
            url,
            miner.getKeyPair ().getPublicKey (),
            miner.getMotto (),
            miner.getVisage ()
        );
    }

    //----------------------------------------------------------------//
    static shared_ptr < AbstractTransaction > makeBody_RegisterMiner ( string accountName, string url, const CryptoPublicKey& publicKey, string motto, const Signature& visage ) {

        shared_ptr < const MinerInfo > minerInfo = make_shared < MinerInfo >(
            url,
            publicKey,
            motto,
            visage
        );

        shared_ptr < Transactions::RegisterMiner > body = make_shared < Transactions::RegisterMiner >();
        body->mAccountName      = accountName;
        body->mMinerInfo        = minerInfo;

        return body;
    }

    //----------------------------------------------------------------//
    static shared_ptr < AbstractTransaction > makeBody_SendVOL ( string to, u64 amount ) {

        shared_ptr < Transactions::SendVOL > body = make_shared < Transactions::SendVOL >();
        body->mAmount       = amount;
        body->mAccountName  = to;

        return body;
    }

    //----------------------------------------------------------------//
    static shared_ptr < TransactionEnvelope > makeTransaction (
        shared_ptr < AbstractTransaction > body,
        string uuid,
        string accountName,
        string keyName              = "master",
        u64 nonce                   = 0,
        u64 gratuity                = 0,
        u64 maxHeight               = 0,
        time_t recordBy             = 0
    ) {
    
        TransactionMaker maker;
        maker.setAccountName ( accountName );
        maker.setGratuity ( gratuity );
        maker.setNonce ( nonce );
        maker.setKeyName ( keyName );
        
        body->setMaker ( maker );
        body->setUUID ( uuid );
        body->setRecordBy ( recordBy );
        body->setMaxHeight ( maxHeight );

        shared_ptr < TransactionEnvelope > transaction = make_shared < TransactionEnvelope >();
        transaction->setBody ( body );

        return transaction;
    }

    //----------------------------------------------------------------//
    static shared_ptr < TransactionEnvelope > makeTransaction (
        shared_ptr < AbstractTransaction > body,
        string uuid,
        const CryptoKeyPair& key,
        string accountName,
        string keyName              = "master",
        u64 nonce                   = 0,
        u64 gratuity                = 0,
        u64 maxHeight               = 0,
        time_t recordBy             = 0
    ) {
    
        shared_ptr < TransactionEnvelope > transaction = makeTransaction (
            body,
            uuid,
            accountName,
            keyName,
            nonce,
            gratuity,
            maxHeight,
            recordBy
        );
        transaction->sign ( key );
        return transaction;
    }
};

} // namespace Simulation
} // namespace Volition
#endif
