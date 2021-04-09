// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/transactions/LoadLedger.h>

namespace Volition {
namespace Transactions {

//================================================================//
// LoadLedgerAccount
//================================================================//

//----------------------------------------------------------------//
TransactionResult LoadLedgerAccount::apply ( AbstractLedger& ledger ) const {
    UNUSED ( ledger );

    Account account;
    account.mBalance    = this->mBalance;
    account.mPolicy     = this->mPolicy;
    account.mBequest    = this->mBequest;
    account.mKeys       = this->mKeys;

    if ( !ledger.newAccount ( this->mName, account )) return "Failed to restore account.";

    AccountID accountID = ledger.getAccountID ( this->mName );
    if ( !ledger.awardAssets ( accountID, this->mInventory, 0 )) return "Failed to restore inventory.";

    if ( this->mMinerInfo ) {
        if ( !ledger.registerMiner (
            ledger.getAccountID ( this->mName ),
            *this->mMinerInfo
        )) return false;
    }

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
void LoadLedger::init ( AbstractLedger& ledger ) {
    
    this->mIdentity     = ledger.getIdentity ();
    this->mSchema       = ledger.getSchema ();
    
    AccountID::Index totalAccounts = ledger.getValue < AccountID::Index >( Ledger::keyFor_globalAccountCount ());
    for ( AccountID::Index i = 0; i < totalAccounts; ++i ) {
    
        AccountODBM accountODBM ( ledger, AccountID ( i ));
        if ( !accountODBM ) continue;
    
        shared_ptr < const Account > account = accountODBM.mBody.get ();
        if ( !account ) continue;
        
        shared_ptr < const MinerInfo > minerInfo = accountODBM.mMinerInfo.get ();
        
        LoadLedgerAccount loadLedgerAccount;
        loadLedgerAccount.mName         = accountODBM.mName.get ( "" );
        loadLedgerAccount.mBalance      = account->mBalance;
        loadLedgerAccount.mPolicy       = account->mPolicy;
        loadLedgerAccount.mBequest      = account->mBequest;
        loadLedgerAccount.mKeys         = account->mKeys;
        loadLedgerAccount.mMinerInfo    = minerInfo ? make_shared < MinerInfo >( *minerInfo ) : NULL;
        
        SerializableList < SerializableSharedConstPtr < Asset >> inventory;
        ledger.getInventory ( accountODBM.mAccountID, inventory, 0, true );
        
        SerializableList < SerializableSharedConstPtr < Asset >>::const_iterator assetIt = inventory.cbegin ();
        for ( ; assetIt != inventory.cend (); ++assetIt ) {
            loadLedgerAccount.mInventory.push_back ( **assetIt );
        }
        
        this->mAccounts.push_back ( loadLedgerAccount );
    }
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void LoadLedger::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    ConsensusSettings::AbstractSerializable_serializeFrom ( serializer );
    AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
    
    serializer.serialize ( "schema",        this->mSchema );
    serializer.serialize ( "accounts",      this->mAccounts );
}

//----------------------------------------------------------------//
void LoadLedger::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    ConsensusSettings::AbstractSerializable_serializeTo ( serializer );
    AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
    
    serializer.serialize ( "schema",        this->mSchema );
    serializer.serialize ( "accounts",      this->mAccounts );
}

//----------------------------------------------------------------//
TransactionResult LoadLedger::AbstractTransactionBody_apply ( TransactionContext& context ) const {
    UNUSED ( context );
    return false;
}

//----------------------------------------------------------------//
TransactionResult LoadLedger::AbstractTransactionBody_genesis ( AbstractLedger& ledger ) const {
    UNUSED ( ledger );

    ledger.setSchema ( this->mSchema );

    SerializableList < LoadLedgerAccount >::const_iterator accountIt = this->mAccounts.cbegin ();
    for ( ; accountIt != this->mAccounts.cend (); ++accountIt ) {
        const LoadLedgerAccount& account = *accountIt;
        LedgerResult result = account.apply ( ledger );
        if ( !result ) return result;
    }
    
    return this->ConsensusSettings::apply ( ledger );
}

} // namespace Transactions
} // namespace Volition
