// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_CONTEXT_H
#define VOLITION_TRANSACTION_CONTEXT_H

#include <volition/common.h>
#include <volition/AccountEntitlements.h>
#include <volition/AccountODBM.h>
#include <volition/KeyEntitlements.h>
#include <volition/SchemaHandle.h>
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

    Ledger&                 mLedger;
    SchemaHandle&           mSchemaHandle;
    AccountODBM&            mAccountODBM;
    const Account           mAccount;
    const KeyAndPolicy&     mKeyAndPolicy;
    time_t                  mTime;
    Entitlements            mAccountEntitlements;
    Entitlements            mKeyEntitlements;

    //----------------------------------------------------------------//
    TransactionContext      ( Ledger& ledger, SchemaHandle& schemaHandle, AccountODBM& accountODBM, const KeyAndPolicy& keyAndPolicy, time_t time );
};

} // namespace Volition
#endif
