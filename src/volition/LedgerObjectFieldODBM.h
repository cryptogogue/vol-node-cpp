// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGEROBJECTFIELDODBM_H
#define VOLITION_LEDGEROBJECTFIELDODBM_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/ConstOpt.h>
#include <volition/Ledger.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// LedgerObjectFieldODBM
//================================================================//
template < typename TYPE >
class LedgerObjectFieldODBM {
private:

    ConstOpt < AbstractLedger >     mLedger;
    LedgerKey                       mKey;

public:

    //----------------------------------------------------------------//
    bool exists () {
            
        return this->mLedger.getConst ().hasValue ( this->mKey );
    }

    //----------------------------------------------------------------//
    shared_ptr < const TYPE > get () {
        
        return this->mLedger.getConst ().template getObjectOrNull < TYPE >( this->mKey );
    }
    
    //----------------------------------------------------------------//
    void get ( TYPE& object ) const {
        
        this->mLedger.getConst ().template getObject < TYPE >( this->mKey, object );
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
    void set ( const TYPE& object ) {
        
        this->mLedger.getMutable ().template setObject < TYPE >( this->mKey, object );
    }
    
    //----------------------------------------------------------------//
    LedgerObjectFieldODBM () {
    }
    
    //----------------------------------------------------------------//
    LedgerObjectFieldODBM ( ConstOpt < AbstractLedger > ledger, const LedgerKey& ledgerKey ) :
        mLedger ( ledger ),
        mKey ( ledgerKey ) {
    }
};

} // namespace Volition
#endif
