// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGERKEY_H
#define VOLITION_LEDGERKEY_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/Ledger.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// LedgerKey
//================================================================//
class LedgerKey {
public:

    typedef std::function < string ()> KeyFunc;

protected:

    string      mKey;
    KeyFunc     mKeyFunc;

public:

    //----------------------------------------------------------------//
    LedgerKey () {
    }

    //----------------------------------------------------------------//
    LedgerKey ( const char* key ) :
        mKey ( key ) {
    }

    //----------------------------------------------------------------//
    LedgerKey ( string key ) :
        mKey ( key ) {
    }
    
    //----------------------------------------------------------------//
    LedgerKey ( const KeyFunc& keyFunc ) :
        mKeyFunc ( keyFunc ) {
    }
    
    //----------------------------------------------------------------//
    operator string () {
        if ( !this->mKey.size () && this->mKeyFunc ) {
            this->mKey = this->mKeyFunc ();
        }
        return this->mKey;
    }
};

} // namespace Volition
#endif
