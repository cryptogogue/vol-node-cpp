// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/SQLite.h>
#include <volition/transactions/LoadLedger.h>

namespace Volition {
namespace Transactions {

//================================================================//
// LoadLedgerAccount
//================================================================//

//----------------------------------------------------------------//
TransactionResult LoadLedgerAccount::apply ( Ledger& ledger ) const {
    UNUSED ( ledger );

//    Account account;
//    account.mBalance = this->mBalance;
//    if ( this->mPolicy ) {
//        account.mPolicy = *this->mPolicy;
//    }
//    account.mBequest = this->mBequest;
//    account.mKeys = this->mKeys;
//
//    if ( !ledger.newAccount ( this->mName, account )) return "Failed to restore account.";
//
//    AccountID accountID = ledger.getAccountID ( this->mName );
//    if ( !ledger.awardAssets ( accountID, this->mInventory, 0 )) return "Failed to restore inventory.";

    return true;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void LoadLedgerAccount::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
    serializer.serialize ( "name",          this->mName );
    serializer.serialize ( "balance",       this->mBalance );
    serializer.serialize ( "policy",        this->mPolicy );
    serializer.serialize ( "bequest",       this->mBequest );
    serializer.serialize ( "keys",          this->mKeys );
    serializer.serialize ( "inventory",     this->mInventory );
    serializer.serialize ( "minerInfo",     this->mMinerInfo );
}

//----------------------------------------------------------------//
void LoadLedgerAccount::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    
    serializer.serialize ( "name",          this->mName );
    serializer.serialize ( "balance",       this->mBalance );
    serializer.serialize ( "policy",        this->mPolicy );
    serializer.serialize ( "bequest",       this->mBequest );
    serializer.serialize ( "keys",          this->mKeys );
    serializer.serialize ( "inventory",     this->mInventory );
    serializer.serialize ( "minerInfo",     this->mMinerInfo );
}

//================================================================//
// LoadLedger
//================================================================//

//----------------------------------------------------------------//
void LoadLedger::init ( Ledger& ledger ) {
    
    this->mIdentity     = ledger.getIdentity ();
    this->mSchema       = ledger.getSchema ();
    
    AccountID::Index totalAccounts = ledger.getValue < AccountID::Index >( Ledger::keyFor_globalAccountCount ());
    for ( AccountID::Index i = 0; i < totalAccounts; ++i ) {
    
        AccountODBM accountODBM ( ledger, AccountID ( i ));
        if ( !accountODBM ) continue;
    
        shared_ptr < const Account > account = accountODBM.mBody.get ();
        if ( !account ) continue;
        
        SerializableList < SerializableSharedConstPtr < Asset >> inventory;
        ledger.getInventory ( accountODBM.mAccountID, inventory, 0, true );
        
//        SerializableList < SerializableSharedConstPtr < Asset >> constInventory;
//        SerializableList < SerializableSharedPtr < Asset >>::const_iterator assetIt = inventory.cbegin ();
//        for ( ; assetIt != inventory.cend (); ++assetIt ) {
//            shared_ptr < const Asset > asset = *assetIt;
//            constInventory.push_back ( asset );
//        }
        
        LoadLedgerAccount loadLedgerAccount;
        loadLedgerAccount.mName         = accountODBM.mName.get ( "" );
        loadLedgerAccount.mBalance      = account->mBalance;
        loadLedgerAccount.mPolicy       = account->mPolicy;
        loadLedgerAccount.mBequest      = account->mBequest;
        loadLedgerAccount.mKeys         = account->mKeys;
        loadLedgerAccount.mInventory    = inventory;
        loadLedgerAccount.mMinerInfo    = accountODBM.mMinerInfo.get ();
        
        this->mAccounts.push_back ( loadLedgerAccount );
    }
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void LoadLedger::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
    
    serializer.serialize ( "identity",      this->mIdentity );
    serializer.serialize ( "schema",        this->mSchema );
    serializer.serialize ( "accounts",      this->mAccounts );
}

//----------------------------------------------------------------//
void LoadLedger::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
    
    serializer.serialize ( "identity",      this->mIdentity );
    serializer.serialize ( "schema",        this->mSchema );
    serializer.serialize ( "accounts",      this->mAccounts );
}

//----------------------------------------------------------------//
TransactionResult LoadLedger::AbstractTransactionBody_apply ( TransactionContext& context ) const {
    UNUSED ( context );
    return false;
}

//----------------------------------------------------------------//
TransactionResult LoadLedger::AbstractTransactionBody_genesis ( Ledger& ledger ) const {
    UNUSED ( ledger );

//    SerializableList < RestoreAccount >::const_iterator accountIt = this->mAccounts.cbegin ();
//    for ( ; accountIt != this->mAccounts.cend (); ++accountIt ) {
//        const RestoreAccount& account = *accountIt;
//        account.apply ( ledger );
//    }
    return true;
}

} // namespace Transactions
} // namespace Volition
