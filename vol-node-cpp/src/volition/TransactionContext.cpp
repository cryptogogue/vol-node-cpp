// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/TransactionContext.h>

namespace Volition {

//================================================================//
// TransactionContext
//================================================================//

//----------------------------------------------------------------//
TransactionContext::TransactionContext ( Ledger& ledger, SchemaHandle& schemaHandle, const Account& account, const KeyAndPolicy& keyAndPolicy, time_t time ) :
    mLedger ( ledger ),
    mSchemaHandle ( schemaHandle ),
    mAccount ( account ),
    mKeyAndPolicy ( keyAndPolicy ),
    mTime ( time ) {
    
    if ( ledger.isGenesis ()) {
        this->mAccountEntitlements = *AccountEntitlements::getMasterEntitlements ();
        this->mKeyEntitlements = *KeyEntitlements::getMasterEntitlements ();
    }
    else {
        this->mAccountEntitlements = ledger.getEntitlements < AccountEntitlements >( account );
        this->mKeyEntitlements = ledger.getEntitlements < KeyEntitlements >( keyAndPolicy );
    }
}

} // namespace Volition
