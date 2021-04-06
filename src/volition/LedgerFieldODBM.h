// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGERFIELDODBM_H
#define VOLITION_LEDGERFIELDODBM_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/ConstOpt.h>
#include <volition/Ledger.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// LedgerFieldODBM
//================================================================//
template < typename TYPE >
class LedgerFieldODBM {
private:

    enum State {
        EMPTY,
        FALLBACK,
        LOADED,
    };

    ConstOpt < AbstractLedger >     mLedger;
    LedgerKey                       mKey;
    State                           mState;
    TYPE                            mValue;

public:

    //----------------------------------------------------------------//
    bool exists () {
            
        return this->mLedger.getConst ().hasValue ( this->mKey );
    }

    //----------------------------------------------------------------//
    TYPE get () {
        
        if ( this->mState != LOADED ) {
            this->mValue = ( this->mState == FALLBACK ) ?
                this->mLedger.getConst ().template getValueOrFallback < TYPE >( this->mKey, this->mValue ) :
                this->mLedger.getConst ().template getValue < TYPE >( this->mKey );
            this->mState = LOADED;
        }
        return this->mValue;
    }
    
    //----------------------------------------------------------------//
    TYPE get ( const TYPE& fallback ) {
        
        if ( this->mState != LOADED ) {
            this->mValue = this->mLedger.getConst ().template getValueOrFallback < TYPE >( this->mKey, fallback );
            this->mState = LOADED;
        }
        return this->mValue;
    }

    //----------------------------------------------------------------//
    void init ( const Ledger& ledger, const LedgerKey& ledgerKey ) {
        this->mLedger.set ( ledger );
        this->mKey = ledgerKey;
    }
    
    //----------------------------------------------------------------//
    void init ( Ledger& ledger, const LedgerKey& ledgerKey ) {
        this->mLedger.set ( ledger );
        this->mKey = ledgerKey;
    }

    //----------------------------------------------------------------//
    void set ( const TYPE& value ) {
        
        if ( !(( this->mState == LOADED ) && ( this->mValue == value ))) {
            this->mLedger.getMutable ().template setValue < TYPE >( this->mKey, value );
            this->mValue = value;
            this->mState = LOADED;
        }
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM () :
        mState ( EMPTY ) {
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM ( ConstOpt < AbstractLedger > ledger, const LedgerKey& ledgerKey ) :
        mLedger ( ledger ),
        mKey ( ledgerKey ),
        mState ( EMPTY ) {
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM ( ConstOpt < AbstractLedger > ledger, const LedgerKey& ledgerKey, TYPE fallback ) :
        mLedger ( ledger ),
        mKey ( ledgerKey ),
        mState ( FALLBACK ),
        mValue ( fallback ) {
    }
};

} // namespace Volition
#endif
