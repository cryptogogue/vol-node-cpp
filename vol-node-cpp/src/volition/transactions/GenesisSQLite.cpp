// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/SQLite.h>
#include <volition/transactions/GenesisSQLite.h>

namespace Volition {
namespace Transactions {

//================================================================//
// RestoreAccount
//================================================================//

//----------------------------------------------------------------//
TransactionResult RestoreAccount::apply ( Ledger& ledger ) const {
            
    Account account;
    account.mBalance = this->mBalance;
    if ( this->mPolicy ) {
        account.mPolicy = *this->mPolicy;
    }
    account.mBequest = this->mBequest;
    account.mKeys = this->mKeys;

    if ( !ledger.newAccount ( this->mName, account )) return "Failed to restore account.";
    
    AccountID accountID = ledger.getAccountID ( this->mName );
    if ( !ledger.awardAssets ( accountID, this->mInventory, 0 )) return "Failed to restore inventory.";

    return true;
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void RestoreAccount::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    
    serializer.serialize ( "name",          this->mName );
    serializer.serialize ( "balance",       this->mBalance );
    serializer.serialize ( "policy",        this->mPolicy );
    serializer.serialize ( "bequest",       this->mBequest );
    serializer.serialize ( "keys",          this->mKeys );
    serializer.serialize ( "inventory",     this->mInventory );
}

//----------------------------------------------------------------//
void RestoreAccount::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    
    serializer.serialize ( "name",          this->mName );
    serializer.serialize ( "balance",       this->mBalance );
    serializer.serialize ( "policy",        this->mPolicy );
    serializer.serialize ( "bequest",       this->mBequest );
    serializer.serialize ( "keys",          this->mKeys );
    serializer.serialize ( "inventory",     this->mInventory );
}

//================================================================//
// GenesisSQLite
//================================================================//

//----------------------------------------------------------------//
void GenesisSQLite::load ( string filename ) {

    SQLite db ( filename );
    assert ( db );
    
    db.exec (
        "SELECT * FROM restore",
        NULL,
        [ this ]( int row, const map < string, int >& columns, sqlite3_stmt* stmt ) {
            UNUSED ( row );
            
            string body = ( cc8* )sqlite3_column_text ( stmt, columns.find ( "body" )->second );
            
            RestoreAccount account;
            FromJSONSerializer::fromJSONString ( account, body );
            
            this->mAccounts.push_back ( account );
        }
    );
}

//================================================================//
// virtual
//================================================================//

//----------------------------------------------------------------//
void GenesisSQLite::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
    AbstractTransactionBody::AbstractSerializable_serializeFrom ( serializer );
    
    serializer.serialize ( "accounts",      this->mAccounts );
}

//----------------------------------------------------------------//
void GenesisSQLite::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
    AbstractTransactionBody::AbstractSerializable_serializeTo ( serializer );
    
    serializer.serialize ( "accounts",      this->mAccounts );
}

//----------------------------------------------------------------//
TransactionResult GenesisSQLite::AbstractTransactionBody_apply ( TransactionContext& context ) const {
    UNUSED ( context );
    return false;
}

//----------------------------------------------------------------//
TransactionResult GenesisSQLite::AbstractTransactionBody_genesis ( Ledger& ledger ) const {
    
    
    SerializableList < RestoreAccount >::const_iterator accountIt = this->mAccounts.cbegin ();
    for ( ; accountIt != this->mAccounts.cend (); ++accountIt ) {
        const RestoreAccount& account = *accountIt;
        account.apply ( ledger );
    }
    return true;
}

} // namespace Transactions
} // namespace Volition
