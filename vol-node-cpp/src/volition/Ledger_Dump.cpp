// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AccountODBM.h>
#include <volition/Asset.h>
#include <volition/AssetMethod.h>
#include <volition/AssetMethodInvocation.h>
#include <volition/AssetODBM.h>
#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/InventoryLogEntry.h>
#include <volition/Ledger.h>
#include <volition/Ledger_Dump.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>
#include <volition/SQLite.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Ledger_Dump
//================================================================//

//----------------------------------------------------------------//
void Ledger_Dump::dump ( string filename ) {

    SQLite db ( filename );
    assert ( db );
    
    db.exec ( "DROP TABLE IF EXISTS accounts" );
    db.exec ( "DROP TABLE IF EXISTS ledger" );
    
    db.exec ( "CREATE TABLE accounts ( id INTEGER PRIMARY KEY, name TEXT NOT NULL, balance INTEGER, keys TEXT NOT NULL, inventory TEXT NOT NULL, policy TEXT NOT NULL, bequest TEXT NOT NULL, minerInfo TEXT NOT NULL )" );
    db.exec ( "CREATE TABLE ledger ( id INTEGER PRIMARY KEY, identity TEXT NOT NULL, schema TEXT NOT NULL )" );
        
    Ledger& ledger = this->getLedger ();
    
    Account::Index totalAccounts = ledger.getValue < Account::Index >( Ledger::keyFor_globalAccountCount ());
    for ( Account::Index i = 0; i < totalAccounts; ++i ) {
    
        shared_ptr < Account > account = ledger.getAccount ( i );
        if ( !account ) continue;
        
        db.exec (
            Format::write ( "REPLACE INTO accounts ( id, name, balance, keys, inventory, policy, bequest, minerInfo ) VALUES ( ?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8 )" ),
            [ &ledger, account, i ]( sqlite3_stmt* stmt ) {
                
                string name = ledger.getAccountName ( i );
                shared_ptr < MinerInfo > minerInfo = ledger.getMinerInfo ( i );
                
                SerializableList < SerializableSharedPtr < Asset >> inventory;
                ledger.getInventory ( ledger.getSchema (), i, inventory, 0, true );
                
                string keysJSON         = ToJSONSerializer::toJSONString ( account->mKeys );
                string inventoryJSON    = ToJSONSerializer::toJSONString ( inventory );
                string policyJSON       = ToJSONSerializer::toJSONString ( account->mPolicy );
                string bequestJSON      = account->mBequest ? ToJSONSerializer::toJSONString ( *account->mBequest ) : "";
                string minerInfoJSON    = minerInfo ? ToJSONSerializer::toJSONString ( *minerInfo ) : "";
            
                sqlite3_bind_int64  ( stmt, 1, ( s64 )account->mIndex );
                sqlite3_bind_text   ( stmt, 2, name.c_str (), ( int )name.size (), SQLITE_TRANSIENT );
                sqlite3_bind_int64  ( stmt, 3, ( s64 )account->mBalance );
                sqlite3_bind_text   ( stmt, 4, keysJSON.c_str (), ( int )keysJSON.size (), SQLITE_TRANSIENT );
                sqlite3_bind_text   ( stmt, 5, inventoryJSON.c_str (), ( int )inventoryJSON.size (), SQLITE_TRANSIENT );
                sqlite3_bind_text   ( stmt, 6, policyJSON.c_str (), ( int )policyJSON.size (), SQLITE_TRANSIENT );
                sqlite3_bind_text   ( stmt, 7, bequestJSON.c_str (), ( int )bequestJSON.size (), SQLITE_TRANSIENT );
                sqlite3_bind_text   ( stmt, 8, minerInfoJSON.c_str (), ( int )minerInfoJSON.size (), SQLITE_TRANSIENT );
            }
        );
    }
    
    db.exec (
        Format::write ( "REPLACE INTO ledger ( id, identity, schema ) VALUES ( ?1, ?2, ?3 )" ),
        [ &ledger ]( sqlite3_stmt* stmt ) {
            
            string schema = ToJSONSerializer::toJSONString ( ledger.getSchema ());
            string identity = ledger.getIdentity ();
            
            sqlite3_bind_int64  ( stmt, 1, 0 );
            sqlite3_bind_text   ( stmt, 2, identity.c_str (), ( int )identity.size (), SQLITE_TRANSIENT ) ;
            sqlite3_bind_text   ( stmt, 3, schema.c_str (), ( int )schema.size (), SQLITE_TRANSIENT ) ;
        }
    );
}

} // namespace Volition
