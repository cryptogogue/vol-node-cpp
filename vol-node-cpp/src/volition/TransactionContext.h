// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_CONTEXT_H
#define VOLITION_TRANSACTION_CONTEXT_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/LedgerODBM.h>
#include <volition/serialization/Serialization.h>
#include <volition/SchemaHandle.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// TransactionContext
//================================================================//
class TransactionContext {
public:

    Ledger&                 mLedger;
    SchemaHandle&           mSchemaHandle;
    const Account&          mAccount;
    const KeyAndPolicy&     mKeyAndPolicy;
    Entitlements            mAccountEntitlements;
    Entitlements            mKeyEntitlements;

    //----------------------------------------------------------------//
    TransactionContext ( Ledger& ledger, SchemaHandle& schemaHandle, const Account& account, const KeyAndPolicy& keyAndPolicy ) :
        mLedger ( ledger ),
        mSchemaHandle ( schemaHandle ),
        mAccount ( account ),
        mKeyAndPolicy ( keyAndPolicy ) {
        
        if ( ledger.isGenesis ()) {
            this->mAccountEntitlements = *AccountEntitlements::getMasterEntitlements ();
            this->mKeyEntitlements = *KeyEntitlements::getMasterEntitlements ();
        }
        else {
            this->mAccountEntitlements = ledger.getEntitlements < AccountEntitlements >( account );
            this->mKeyEntitlements = ledger.getEntitlements < KeyEntitlements >( keyAndPolicy );
        }
    }
};

} // namespace Volition
#endif
