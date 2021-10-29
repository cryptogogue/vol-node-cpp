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
LedgerResult TransactionContext::pushAccountLogEntry () {

    return this->pushAccountLogEntry ( this->mAccountID );
}

//----------------------------------------------------------------//
LedgerResult TransactionContext::pushAccountLogEntry ( AccountID accountID ) {

    return this->mLedger.pushAccountLogEntry ( accountID, AccountLogEntry ( this->mBlockHeight, this->mIndex ));
}

//----------------------------------------------------------------//
TransactionContext::TransactionContext ( AbstractLedger& ledger, AccountODBM& accountODBM, const KeyAndPolicy& keyAndPolicy, u64 blockHeight, u64 blockVersion, u64 index, time_t time ) :
    mAccount ( *accountODBM.mBody.get ()),
    mAccountID ( accountODBM.mAccountID ),
    mAccountODBM ( accountODBM ),
    mKeyAndPolicy ( keyAndPolicy ),
    mLedger ( ledger ),
    mBlockHeight ( blockHeight ),
    mBlockVersion ( blockVersion ),
    mIndex ( index ),
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
