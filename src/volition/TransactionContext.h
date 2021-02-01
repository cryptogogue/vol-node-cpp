// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_CONTEXT_H
#define VOLITION_TRANSACTION_CONTEXT_H

#include <volition/common.h>
#include <volition/AccountEntitlements.h>
#include <volition/AccountODBM.h>
#include <volition/FeeSchedule.h>
#include <volition/KeyEntitlements.h>
#include <volition/serialization/Serialization.h>
#include <volition/TransactionMaker.h>

namespace Volition {

class Account;
class KeyAndPolicy;
class Ledger;

//================================================================//
// TransactionContext
//================================================================//
class TransactionContext {
public:

    const Account           mAccount;
    Entitlements            mAccountEntitlements;
    AccountODBM&            mAccountODBM;
    FeeSchedule             mFeeSchedule;
    AccountID               mIndex;
    const KeyAndPolicy&     mKeyAndPolicy;
    Entitlements            mKeyEntitlements;
    Ledger&                 mLedger;
    time_t                  mTime;

    //----------------------------------------------------------------//
    TransactionContext      ( Ledger& ledger, AccountODBM& accountODBM, const KeyAndPolicy& keyAndPolicy, time_t time );
};

} // namespace Volition
#endif
