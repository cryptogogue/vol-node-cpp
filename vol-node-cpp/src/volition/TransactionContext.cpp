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
TransactionContext::TransactionContext ( Ledger& ledger, SchemaHandle& schemaHandle, AccountODBM& accountODBM, const KeyAndPolicy& keyAndPolicy, time_t time ) :
    mLedger ( ledger ),
    mSchemaHandle ( schemaHandle ),
    mIndex ( accountODBM.mAccountID ),
    mAccountODBM ( accountODBM ),
    mAccount ( *accountODBM.mBody.get ()),
    mKeyAndPolicy ( keyAndPolicy ),
    mTime ( time ) {
    
    if ( ledger.isGenesis ()) {
        this->mAccountEntitlements = *AccountEntitlements::getMasterEntitlements ();
        this->mKeyEntitlements = *KeyEntitlements::getMasterEntitlements ();
    }
    else {
        this->mAccountEntitlements = ledger.getEntitlements < AccountEntitlements >( *accountODBM.mBody.get ());
        this->mKeyEntitlements = ledger.getEntitlements < KeyEntitlements >( keyAndPolicy );
    }
}

} // namespace Volition
