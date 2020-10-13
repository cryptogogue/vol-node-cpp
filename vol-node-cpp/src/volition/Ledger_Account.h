// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_ACCOUNT_H
#define VOLITION_LEDGER_ACCOUNT_H

#include <volition/common.h>
#include <volition/AbstractLedgerComponent.h>
#include <volition/Account.h>
#include <volition/AccountEntitlements.h>
#include <volition/AccountKeyLookup.h>
#include <volition/Asset.h>
#include <volition/LedgerKey.h>
#include <volition/LedgerResult.h>

namespace Volition {

class InventoryLogEntry;

//================================================================//
// AccountKey
//================================================================//
class AccountKey {
public:
    
    shared_ptr < const Account >        mAccount;
    const KeyAndPolicy*                 mKeyAndPolicy;
    
    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mAccount && this->mKeyAndPolicy );
    }
};

//================================================================//
// Ledger_Account
//================================================================//
class Ledger_Account :
    virtual public AbstractLedgerComponent {
private:

    //----------------------------------------------------------------//
    static LedgerKey keyFor_reservedName ( string nameHash ) {
        assert ( nameHash.size () > 0 );
        return Format::write ( "reserve.nameHash.%s", nameHash.c_str ());
    }

    //----------------------------------------------------------------//
    static bool                         isAccountNameChar               ( char c );

public:

    static constexpr const char* MASTER_KEY_NAME = "master";

    //----------------------------------------------------------------//
    bool                                affirmKey                       ( Account::Index accountIndex, string makerKeyName, string keyName, const CryptoKey& key, const Policy* policy );
    LedgerResult                        awardVOL                        ( Account::Index accountIndex, u64 amount );
    bool                                deleteKey                       ( Account::Index accountIndex, string keyName );
    Account::Index                      getAccountIndex                 ( string accountName ) const;
    AccountKey                          getAccountKey                   ( Account::Index accountIndex, string keyName ) const;
    shared_ptr < AccountKeyLookup >     getAccountKeyLookup             ( string keyID ) const;
    static bool                         isAccountName                   ( string accountName );
    static bool                         isChildName                     ( string accountName );
    static bool                         isSuffix                        ( string suffix );
    bool                                newAccount                      ( string accountName, u64 balance, string keyName, const CryptoKey& key, const Policy& keyPolicy, const Policy& accountPolicy );
    LedgerResult                        renameAccount                   ( Account::Index accountIndex, string revealedName );
    LedgerResult                        reserveAccountname              ( string nameHash, string nameSecret );
};

} // namespace Volition
#endif
