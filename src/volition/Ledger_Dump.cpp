// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Ledger.h>
#include <volition/Ledger_Dump.h>
#include <volition/serialization/Serialization.h>
#include <volition/Transactions.h>

namespace Volition {

//================================================================//
// Ledger_Dump
//================================================================//

//----------------------------------------------------------------//
void Ledger_Dump::dump ( string filename ) {

    AbstractLedger& ledger = this->getLedger ();

    Transactions::LoadLedger loadLedger;
    loadLedger.init ( ledger );
    
    ToJSONSerializer::toJSONFile ( loadLedger, filename );
}

} // namespace Volition
