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
#include <volition/LedgerResult.h>

namespace Volition {

//================================================================//
// AccountKey
//================================================================//
class AccountKey {
public:
    
    shared_ptr < Account >              mAccount;
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
public:

    static constexpr const char* MASTER_KEY_NAME    = "master";

    //----------------------------------------------------------------//
    bool                                affirmKey                   ( string accountName, string makerKeyName, string keyName, const CryptoKey& key, const Policy* policy );
    bool                                deleteKey                   ( string accountName, string keyName );
    shared_ptr < Account >              getAccount                  ( Account::Index index ) const;
    shared_ptr < Account >              getAccount                  ( string accountName ) const;
    Account::Index                      getAccountIndex             ( string accountName ) const;
    AccountKey                          getAccountKey               ( string accountName, string keyName ) const;
    shared_ptr < AccountKeyLookup >     getAccountKeyLookup         ( string keyID ) const;
    string                              getAccountName              ( Account::Index accountIndex ) const;
    u64                                 getAccountInventoryNonce    ( Account::Index accountIndex ) const;
    u64                                 getAccountTransactionNonce  ( Account::Index accountIndex ) const;
    void                                incAccountInventoryNonce    ( Account::Index index, u64 nonce );
    void                                incAccountTransactionNonce  ( Account::Index index, u64 nonce, string note );
    static bool                         isAccountName               ( string accountName );
    static bool                         isChildName                 ( string accountName );
    static bool                         isSuffix                    ( string suffix );
    bool                                newAccount                  ( string accountName, u64 balance, string keyName, const CryptoKey& key, const Policy& keyPolicy, const Policy& accountPolicy );
    void                                setAccount                  ( const Account& account );    
};

} // namespace Volition
#endif
