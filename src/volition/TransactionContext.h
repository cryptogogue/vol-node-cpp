// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_CONTEXT_H
#define VOLITION_TRANSACTION_CONTEXT_H

#include <volition/common.h>
#include <volition/AccountEntitlements.h>
#include <volition/AccountODBM.h>
#include <volition/TransactionFeeSchedule.h>
#include <volition/KeyEntitlements.h>
#include <volition/serialization/Serialization.h>
#include <volition/TransactionMaker.h>

namespace Volition {

class Account;
class KeyAndPolicy;
class Ledger;

//================================================================//
// TransactionContext
class TransactionContext {
//================================================================//
public:

    TransactionFeeSchedule  mFeeSchedule;
    AbstractLedger&         mLedger;
    u64                     mBlockHeight;
    u64                     mRelease;
    u64                     mIndex;
    time_t                  mTime;
    
    AccountID               mAccountID;
    AccountODBM             mAccountODBM;
    Account                 mAccount;
    Entitlements            mAccountEntitlements;
    Entitlements            mKeyEntitlements;
    KeyAndPolicy            mKeyAndPolicy;
    TransactionMaker        mMaker;

    IS ( Genesis,           mLedger.isGenesis (),       true )
    IS ( Provisional,       mAccountID,                 AccountID::NULL_INDEX )

    //----------------------------------------------------------------//
    LedgerResult            pushAccountLogEntry         ();
    LedgerResult            pushAccountLogEntry         ( AccountID accountID );
                            TransactionContext          ( AbstractLedger& ledger, const TransactionMaker& maker, u64 blockHeight, u64 release, u64 index, time_t time );
};

} // namespace Volition
#endif
