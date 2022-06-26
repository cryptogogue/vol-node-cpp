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
TransactionContext::TransactionContext ( AbstractLedger& ledger, const TransactionMaker& maker, u64 blockHeight, u64 release, u64 index, time_t time ) :
    mLedger ( ledger ),
    mBlockHeight ( blockHeight ),
    mRelease ( release ),
    mIndex ( index ),
    mTime ( time ),
    mAccountID ( AccountID::NULL_INDEX ),
    mAccountODBM ( ledger, maker.getAccountName ()),
    mMaker ( maker ) {
    
    if ( this->mAccountODBM ) {
        
        this->mAccountID        = this->mAccountODBM.mAccountID;
        this->mAccount          = *this->mAccountODBM.mBody.get ();
        this->mKeyAndPolicy     = this->mAccountODBM.getKeyAndPolicyOrNull ( maker.getKeyName ());
    
        if ( ledger.isGenesis ()) {
            this->mAccountEntitlements  = *AccountEntitlements::getMasterEntitlements ();
            this->mKeyEntitlements      = *KeyEntitlements::getMasterEntitlements ();
        }
        else {
            this->mAccountEntitlements  = ledger.getEntitlements < AccountEntitlements >( *this->mAccountODBM.mBody.get ());
            this->mKeyEntitlements      = ledger.getEntitlements < KeyEntitlements >( this->mKeyAndPolicy );
        }
    }
    
    this->mFeeSchedule = ledger.getTransactionFeeSchedule ();
}

} // namespace Volition
