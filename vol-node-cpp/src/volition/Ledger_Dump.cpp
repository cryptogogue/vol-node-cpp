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
    
    db.exec ( "CREATE TABLE IF NOT EXISTS accounts ( id INTEGER PRIMARY KEY, name TEXT NOT NULL, balance INTEGER, policy TEXT NOT NULL, bequest TEXT NOT NULL )" );
    db.exec ( "CREATE TABLE IF NOT EXISTS keys ( id INTEGER PRIMARY KEY, ownerID INTEGER, body TEXT NOT NULL, policy TEXT NOT NULL, bequest TEXT NOT NULL )" );
    db.exec ( "CREATE TABLE IF NOT EXISTS assets ( id INTEGER PRIMARY KEY, ownerID INTEGER, body TEXT NOT NULL, policy TEXT NOT NULL, bequest TEXT NOT NULL )" );
    
    Ledger& ledger = this->getLedger ();
    Account::Index totalAccounts = ledger.getValue < Account::Index >( Ledger::keyFor_globalAccountCount ());
    for ( Account::Index i = 0; i < totalAccounts; ++i ) {
    
        shared_ptr < Account > account = ledger.getAccount ( i );
        if ( !account ) continue;
        
        string name = ledger.getAccountName ( i );
        
        db.exec (
            Format::write ( "REPLACE INTO accounts ( id, name, balance, policy, bequest ) VALUES ( ?1, ?2, ?3, ?4, ?5 )" ),
            [ &account, name ]( sqlite3_stmt* stmt ) {
                
                string policy   = ToJSONSerializer::toJSONString ( account->mPolicy );
                string bequest  = account->mBequest ? ToJSONSerializer::toJSONString ( *account->mBequest ) : "";
            
                sqlite3_bind_int64  ( stmt, 1, ( s64 )account->mIndex );
                sqlite3_bind_text   ( stmt, 2, name.c_str (), ( int )name.size (), SQLITE_TRANSIENT );
                sqlite3_bind_int64  ( stmt, 3, ( s64 )account->mBalance );
                sqlite3_bind_text   ( stmt, 4, policy.c_str (), ( int )policy.size (), SQLITE_TRANSIENT );
                sqlite3_bind_text   ( stmt, 5, bequest.c_str (), ( int )bequest.size (), SQLITE_TRANSIENT );
            }
        );
    }
}

} // namespace Volition
