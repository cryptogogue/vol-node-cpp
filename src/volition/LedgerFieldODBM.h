// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGERODBM_H
#define VOLITION_LEDGERODBM_H

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

    ConstOpt < Ledger >     mLedger;
    LedgerKey               mKey;
    bool                    mIsLoaded;
    TYPE                    mValue;

public:

    //----------------------------------------------------------------//
    bool exists () const {
            
        return this->mLedger.getConst ().hasValue ( this->mKey );
    }

    //----------------------------------------------------------------//
    TYPE get () {
        
        if ( !this->mIsLoaded ) {
            this->mValue = this->mLedger.getConst ().template getValue < TYPE >( this->mKey );
            this->mIsLoaded = true;
        }
        return this->mValue;
    }
    
    //----------------------------------------------------------------//
    TYPE get ( const TYPE& fallback ) {
        
        if ( !this->mIsLoaded ) {
            this->mValue = this->mLedger.getConst ().template getValueOrFallback < TYPE >( this->mKey, fallback );
            this->mIsLoaded = true;
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
        
        if ( !( this->mIsLoaded && ( this->mValue == value ))) {
            this->mLedger.getMutable ().template setValue < TYPE >( this->mKey, value );
            this->mValue = value;
            this->mIsLoaded = true;
        }
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM () :
        mIsLoaded ( false ) {
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM ( ConstOpt < Ledger > ledger, const LedgerKey& ledgerKey ) :
        mLedger ( ledger ),
        mKey ( ledgerKey ),
        mIsLoaded ( false ) {
    }
};

} // namespace Volition
#endif
