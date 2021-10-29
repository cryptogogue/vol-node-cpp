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

    ConstOpt < AbstractLedger >     mLedger;
    LedgerKey                       mKey;
    bool                            mHasFallback;
    TYPE                            mFallback;

public:

    //----------------------------------------------------------------//
    bool exists () {
            
        return this->mLedger.getConst ().hasValue ( this->mKey );
    }

    //----------------------------------------------------------------//
    TYPE get () {
        
        return ( this->mHasFallback ) ?
            this->mLedger.getConst ().template getValueOrFallback < TYPE >( this->mKey, this->mFallback ) :
            this->mLedger.getConst ().template getValue < TYPE >( this->mKey );
    }
    
    //----------------------------------------------------------------//
    TYPE get ( const TYPE& fallback ) {

        return this->mLedger.getConst ().template getValueOrFallback < TYPE >( this->mKey, fallback );
    }

    //----------------------------------------------------------------//
    void init ( const AbstractLedger& ledger, const LedgerKey& ledgerKey ) {
        this->mLedger.set ( ledger );
        this->mKey = ledgerKey;
    }
    
    //----------------------------------------------------------------//
    void init ( AbstractLedger& ledger, const LedgerKey& ledgerKey ) {
        this->mLedger.set ( ledger );
        this->mKey = ledgerKey;
    }

    //----------------------------------------------------------------//
    void set ( const TYPE& value ) {
        
        this->mLedger.getMutable ().template setValue < TYPE >( this->mKey, value );
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM () :
        mHasFallback ( false ) {
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM ( ConstOpt < AbstractLedger > ledger, const LedgerKey& ledgerKey ) :
        mLedger ( ledger ),
        mKey ( ledgerKey ),
        mHasFallback ( false ) {
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM ( ConstOpt < AbstractLedger > ledger, const LedgerKey& ledgerKey, TYPE fallback ) :
        mLedger ( ledger ),
        mKey ( ledgerKey ),
        mHasFallback ( true ),
        mFallback ( fallback ) {
    }
};

} // namespace Volition
#endif
