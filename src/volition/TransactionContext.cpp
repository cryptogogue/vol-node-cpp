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
TransactionContext::TransactionContext ( AbstractLedger& ledger, AccountODBM& accountODBM, const KeyAndPolicy& keyAndPolicy, time_t time ) :
    mAccount ( *accountODBM.mBody.get ()),
    mAccountID ( accountODBM.mAccountID ),
    mAccountODBM ( accountODBM ),
    mKeyAndPolicy ( keyAndPolicy ),
    mLedger ( ledger ),
    mTime ( time ) {
    
    if ( ledger.isGenesis ()) {
        this->mAccountEntitlements = *AccountEntitlements::getMasterEntitlements ();
        this->mKeyEntitlements = *KeyEntitlements::getMasterEntitlements ();
    }
    else {
        this->mAccountEntitlements = ledger.getEntitlements < AccountEntitlements >( *accountODBM.mBody.get ());
        this->mKeyEntitlements = ledger.getEntitlements < KeyEntitlements >( keyAndPolicy );
    }
    
    this->mFeeSchedule = ledger.getTransactionFeeSchedule ();
}

} // namespace Volition
