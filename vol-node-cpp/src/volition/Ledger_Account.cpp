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
bool Ledger_Account::affirmKey ( Account::Index accountIndex, string makerKeyName, string keyName, const CryptoKey& key, const Policy* policy ) {

    Ledger& ledger = this->getLedger ();

    string keyID = key.getKeyID ();
    if ( keyID.size ()) return false;

    AccountODBM accountODBM ( ledger, accountIndex );
    if ( !accountODBM ) return false;

    shared_ptr < const Account > account = accountODBM.mBody.get ();

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
        
        Account updatedAccount = *account;
        updatedAccount.mKeys [ keyName ] = KeyAndPolicy ( key, *selectedPolicy );
        accountODBM.mBody.set ( updatedAccount );
        
        ledger.setObject < AccountKeyLookup >( KEY_FOR_ACCOUNT_KEY_LOOKUP, AccountKeyLookup ( account->mIndex, keyName ));
        
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Account::awardVOL ( Account::Index accountIndex, u64 amount ) {

    Ledger& ledger = this->getLedger ();

    AccountODBM accountODBM ( ledger, accountIndex );

    shared_ptr < const Account > account = accountODBM.mBody.get ();
    if ( account ) {
        
        u64 created = ledger.createVOL ( amount );
        if ( created < amount ) return "New VOL request exceeds ledger maximum.";
    
        Account updatedAccount = *account;
        updatedAccount.mBalance += created;
        accountODBM.mBody.set ( updatedAccount );
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger_Account::deleteKey ( Account::Index accountIndex, string keyName ) {

    Ledger& ledger = this->getLedger ();

    AccountKey accountKey = ledger.getAccountKey ( accountIndex, keyName );
    if ( accountKey ) {
        Account updatedAccount = *accountKey.mAccount;
        updatedAccount.mKeys.erase ( keyName );
        AccountODBM ( ledger, accountIndex ).mBody.set ( updatedAccount );
        return true;
    }
    return false;
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
AccountKey Ledger_Account::getAccountKey ( Account::Index accountIndex, string keyName ) const {

    const Ledger& ledger = this->getLedger ();

    AccountKey accountKey;
    accountKey.mKeyAndPolicy = NULL;

    accountKey.mAccount = AccountODBM ( ledger, accountIndex ).mBody.get ();
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
bool Ledger_Account::isAccountName ( string accountName ) {

    size_t size = accountName.size ();
    for ( size_t i = 0; i < size; ++i ) {
        const char c = accountName [ i ];
        if ( !Ledger_Account::isAccountNameChar ( c )) return false;
    }
    return true;
}

//----------------------------------------------------------------//
bool Ledger_Account::isAccountNameChar ( char c ) {

    return  (( c >= '0' ) && ( c <= '9' )) ||
            (( c >= 'a' ) && ( c <= 'z' )) ||
            (( c >= 'A' ) && ( c <= 'Z' )) ||
            ( c == '.' ) || ( c == '-' );
}

//----------------------------------------------------------------//
bool Ledger_Account::isChildName ( string accountName ) {

    return ( accountName [ 0 ] == '.' );
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
    account.mBalance    = balance;
    
    if ( key ) {

        account.mKeys [ MASTER_KEY_NAME ] = KeyAndPolicy ( key, keyPolicy );

        // store the key (for reverse lookup):
        string keyID = key.getKeyID ();
        assert ( keyID.size ());
        ledger.setObject < AccountKeyLookup >( Ledger::keyFor_accountKeyLookup ( keyID ), AccountKeyLookup ( accountIndex, keyName ));
    }
    
    AccountODBM accountODBM ( ledger, accountIndex );
    accountODBM.mName.set ( accountName );
    accountODBM.mBody.set ( account );
    
    // store the alias
    ledger.setValue < Account::Index >( KEY_FOR_ACCOUNT_ALIAS, accountIndex );

    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Account::renameAccount ( Account::Index accountIndex, string revealedName ) {

    Ledger& ledger = this->getLedger ();
    
    AccountODBM accountODBM ( ledger, accountIndex );
    
    if ( !accountODBM ) return "Account not found.";
    string accountName = accountODBM.mName.get ();

    // make sure the revealed name is valid
    if ( Ledger::isChildName ( revealedName )) return "Renamed accounts must not begin with '.'"; // new names must not begin with a '~'
    if ( !Ledger::isAccountName ( revealedName )) return "Proposed account name contains invalid characters."; // make sure it's a valid account name

    string lowerRevealedName = Format::tolower ( revealedName );
    
    // check to see if the alias already exists
    LedgerFieldODBM < Account::Index > alias = LedgerFieldODBM < Account::Index > ( ledger, Ledger::keyFor_accountAlias ( lowerRevealedName ));
    if ( alias.exists ()) {
    
        // error if alias isn't owned by this account
        if ( alias.get () != accountIndex ) return "Alias exists and belongs to another account.";
    }
    else {
    
        // creating a new alias, so make sure name hasn't been reserved
        string nameHash = Digest ( lowerRevealedName ).toHex ();
        LedgerFieldODBM < string > reservedNameField = LedgerFieldODBM < string > ( ledger, keyFor_reservedName ( nameHash ));
        
        if ( reservedNameField.exists ()) {
        
            string lowerAccountName = Format::tolower ( accountName );
            string nameSecret = Digest ( Format::write ( "%s:%s", lowerAccountName.c_str (), lowerRevealedName.c_str ())).toHex ();
            
            if ( reservedNameField.get () != nameSecret ) return Format::write ( "Account name \"%s\" has already been claimed.", revealedName.c_str ());
        }
        
        // claim the alias
        alias.set ( accountIndex );
    }

    // update the account name
//    Account accountUpdated = *account;
//    accountUpdated.mName = revealedName;
//    ledger.setObject < Account >( AccountODBM::keyFor_body ( account->mIndex ), accountUpdated );
//    ledger.setValue < string >( AccountODBM::keyFor_name ( account->mIndex ), revealedName );
    
    accountODBM.mName.set ( revealedName );
    
    return true;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Account::reserveAccountname ( string nameHash, string nameSecret ) {

    Ledger& ledger = this->getLedger ();

    LedgerFieldODBM < string > reservedNameField = LedgerFieldODBM < string > ( ledger, keyFor_reservedName ( nameHash ));
    if ( reservedNameField.exists ()) return Format::write ( "Account name has already been reserved." );
    
    reservedNameField.set ( nameSecret );
    
    return true;
}

} // namespace Volition
