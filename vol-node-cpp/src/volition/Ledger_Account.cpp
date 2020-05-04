// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Asset.h>
#include <volition/AssetMethod.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/AssetODBM.h>
#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/Ledger_Account.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Ledger_Account
//================================================================//

//----------------------------------------------------------------//
bool Ledger_Account::affirmKey ( string accountName, string makerKeyName, string keyName, const CryptoKey& key, const Policy* policy ) {

    Ledger& ledger = this->getLedger ();

    string keyID = key.getKeyID ();
    if ( keyID.size ()) return false;

    shared_ptr < Account > account = ledger.getAccount ( accountName );
    if ( account ) {

        const LedgerKey KEY_FOR_ACCOUNT_KEY_LOOKUP = Ledger::keyFor_accountKeyLookup ( keyID );
        shared_ptr < AccountKeyLookup > accountKeyLookup = ledger.getObjectOrNull < AccountKeyLookup >( KEY_FOR_ACCOUNT_KEY_LOOKUP );

        // keys must be unique to accounts; no sharing keys across multiple accounts!
        if ( accountKeyLookup && ( accountKeyLookup->mAccountIndex != account->mIndex )) return false;

        if ( key ) {
            
            const KeyAndPolicy* makerKeyAndPolicy = account->getKeyAndPolicyOrNull ( makerKeyName );
            if ( !makerKeyAndPolicy ) return false;
            
            const Policy* selectedPolicy = policy;
            if ( selectedPolicy ) {
                if ( !ledger.isValidPolicy < KeyEntitlements >( *selectedPolicy, makerKeyAndPolicy->mPolicy )) return false;
            }
            else {
                const KeyAndPolicy* keyAndPolicy = account->getKeyAndPolicyOrNull ( makerKeyName );
                selectedPolicy = keyAndPolicy ? &keyAndPolicy->mPolicy : &makerKeyAndPolicy->mPolicy;
            }
            
            account->mKeys [ keyName ] = KeyAndPolicy ( key, *selectedPolicy );
            ledger.setAccount ( *account );
            
            ledger.setObject < AccountKeyLookup >( KEY_FOR_ACCOUNT_KEY_LOOKUP, AccountKeyLookup ( account->mIndex, keyName ));
            
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger_Account::deleteKey ( string accountName, string keyName ) {

    Ledger& ledger = this->getLedger ();

    AccountKey accountKey = ledger.getAccountKey ( accountName, keyName );
    if ( accountKey ) {
        Account updatedAccount = *accountKey.mAccount;
        updatedAccount.mKeys.erase ( keyName );
        ledger.setAccount ( updatedAccount );
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
shared_ptr < Account > Ledger_Account::getAccount ( Account::Index index ) const {

    const Ledger& ledger = this->getLedger ();
    return ledger.getObjectOrNull < Account >( AccountODBM::keyFor_body ( index ));
}

//----------------------------------------------------------------//
shared_ptr < Account > Ledger_Account::getAccount ( string accountName ) const {

    const Ledger& ledger = this->getLedger ();

    Account::Index accountIndex = ledger.getAccountIndex ( accountName );
    if ( accountIndex == Account::NULL_INDEX ) return NULL;
    return ledger.getAccount ( accountIndex );
}

//----------------------------------------------------------------//
Account::Index Ledger_Account::getAccountIndex ( string accountName ) const {

    const Ledger& ledger = this->getLedger ();
    if ( accountName.size () == 0 ) return Account::NULL_INDEX;

    string lowerName = Format::tolower ( accountName );
    LedgerKey KEY_FOR_ACCOUNT_ALIAS = Ledger::keyFor_accountAlias ( lowerName );
    return ledger.getValueOrFallback < Account::Index >( KEY_FOR_ACCOUNT_ALIAS, Account::NULL_INDEX );
}

//----------------------------------------------------------------//
AccountKey Ledger_Account::getAccountKey ( string accountName, string keyName ) const {

    const Ledger& ledger = this->getLedger ();

    AccountKey accountKey;
    accountKey.mKeyAndPolicy = NULL;

    accountKey.mAccount = ledger.getAccount ( accountName );
    if ( accountKey.mAccount ) {
        map < string, KeyAndPolicy >::const_iterator keyAndPolicyIt = accountKey.mAccount->mKeys.find ( keyName );
        if ( keyAndPolicyIt != accountKey.mAccount->mKeys.cend ()) {
            accountKey.mKeyAndPolicy = &keyAndPolicyIt->second;
        }
    }
    return accountKey;
}

//----------------------------------------------------------------//
shared_ptr < AccountKeyLookup > Ledger_Account::getAccountKeyLookup ( string keyID ) const {

    const Ledger& ledger = this->getLedger ();
    return ledger.getObjectOrNull < AccountKeyLookup >( Ledger::keyFor_accountKeyLookup ( keyID ));
}

//----------------------------------------------------------------//
string Ledger_Account::getAccountName ( Account::Index accountIndex ) const {

    const Ledger& ledger = this->getLedger ();
    return ledger.getValueOrFallback < string >( AccountODBM::keyFor_name ( accountIndex ), "" );
}

//----------------------------------------------------------------//
u64 Ledger_Account::getAccountInventoryNonce ( Account::Index accountIndex ) const {

    const Ledger& ledger = this->getLedger ();
    return ledger.getValueOrFallback < u64 >( AccountODBM::keyFor_inventoryNonce ( accountIndex ), 0 );
}

//----------------------------------------------------------------//
u64 Ledger_Account::getAccountTransactionNonce ( Account::Index accountIndex ) const {

    const Ledger& ledger = this->getLedger ();
    return ledger.getValueOrFallback < u64 >( AccountODBM::keyFor_transactionNonce ( accountIndex ), 0 );
}

//----------------------------------------------------------------//
void Ledger_Account::incAccountInventoryNonce ( Account::Index index, u64 nonce ) {

    Ledger& ledger = this->getLedger ();

    AccountODBM accountODBM ( ledger, index );
    if ( !accountODBM.mBody.exists ()) return;
    if ( accountODBM.mInventoryNonce.get ( 0 ) != nonce ) return;
    
    accountODBM.mInventoryNonce.set ( nonce + 1 );
}

//----------------------------------------------------------------//
void Ledger_Account::incAccountTransactionNonce ( Account::Index index, u64 nonce, string note ) {

    Ledger& ledger = this->getLedger ();

    AccountODBM accountODBM ( ledger, index );
    if ( !accountODBM.mBody.exists ()) return;
    if ( accountODBM.mTransactionNonce.get ( 0 ) != nonce ) return;
    
    LedgerKey KEY_FOR_ACCOUNT_TRANSACTION_NOTE = AccountODBM::keyFor_transactionNoteField ( index, nonce );
    ledger.setValue < string >( KEY_FOR_ACCOUNT_TRANSACTION_NOTE, note );

    accountODBM.mTransactionNonce.set ( nonce + 1 );
}

//----------------------------------------------------------------//
bool Ledger_Account::isAccountName ( string accountName ) {

    size_t size = accountName.size ();
    for ( size_t i = 0; i < size; ++i ) {
        const char c = accountName [ i ];
        if ( !isgraph ( c )) return false;
    }
    return true;
}

//----------------------------------------------------------------//
bool Ledger_Account::isChildName ( string accountName ) {

    return ( accountName [ 0 ] == '~' );
}

//----------------------------------------------------------------//
bool Ledger_Account::isSuffix ( string suffix ) {

    // child names must follow the format "<hex3>.<hex3>.<hex3>", where each hex3 is a *lowecase* 3-digit hexadecimal number.
    
    if ( suffix.size () != 11 ) return false;
    if (( suffix [ 3 ] != '.' ) || ( suffix [ 7 ] != '.' )) return false;
    
    for ( size_t i = 0; i < 11; ++i ) {
    
        if (( i == 3 ) || ( i == 7 )) continue;
    
        char c = suffix [ i ];
    
        if (( '0' <= c ) || ( c <= '9' )) continue;
        if (( 'a' <= c ) || ( c <= 'f' )) continue;
        
        return false;
    }
    return true;
}

//----------------------------------------------------------------//
bool Ledger_Account::newAccount ( string accountName, u64 balance, string keyName, const CryptoKey& key, const Policy& keyPolicy, const Policy& accountPolicy ) {

    Ledger& ledger = this->getLedger ();

    // check to see if there is already an alias for this account name
    string lowerName = Format::tolower ( accountName );
    LedgerKey KEY_FOR_ACCOUNT_ALIAS = Ledger::keyFor_accountAlias ( lowerName );
    if ( ledger.hasKey ( KEY_FOR_ACCOUNT_ALIAS )) return false; // alias already exists

    // provision the account ID
    LedgerKey KEY_FOR_GLOBAL_ACCOUNT_COUNT = Ledger::keyFor_globalAccountCount ();
    Account::Index accountIndex = ledger.getValue < Account::Index >( KEY_FOR_GLOBAL_ACCOUNT_COUNT );
    ledger.setValue < Account::Index >( KEY_FOR_GLOBAL_ACCOUNT_COUNT, accountIndex + 1 ); // increment counter

    // store the account
    Account account;
    account.mPolicy     = accountPolicy;
    account.mIndex      = accountIndex;
    account.mName       = accountName;
    account.mBalance    = balance;
    account.mKeys [ MASTER_KEY_NAME ] = KeyAndPolicy ( key, keyPolicy );
    
    ledger.setObject < Account >( AccountODBM::keyFor_body ( accountIndex ), account );

    // store the key (for reverse lookup):
    string keyID = key.getKeyID ();
    assert ( keyID.size ());
    ledger.setObject < AccountKeyLookup >( Ledger::keyFor_accountKeyLookup ( keyID ), AccountKeyLookup ( accountIndex, keyName ));

    // store the alias
    ledger.setValue < Account::Index >( KEY_FOR_ACCOUNT_ALIAS, accountIndex );
    ledger.setValue < string >( AccountODBM::keyFor_name ( accountIndex ), accountName );

    return true;
}

//----------------------------------------------------------------//
void Ledger_Account::setAccount ( const Account& account ) {

    Ledger& ledger = this->getLedger ();

    assert ( account.mIndex != Account::NULL_INDEX );
    ledger.setObject < Account >( AccountODBM::keyFor_body ( account.mIndex ), account );
}

} // namespace Volition
