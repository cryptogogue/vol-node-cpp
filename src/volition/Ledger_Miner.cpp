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
#include <volition/Ledger_Miner.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LuaContext.h>
#include <volition/TransactionMaker.h>

namespace Volition {

//================================================================//
// Ledger_Miner
//================================================================//

//----------------------------------------------------------------//
set < string > Ledger_Miner::getMiners () const {

    const AbstractLedger& ledger = this->getLedger ();

    set < string > minerInfoSet;

    shared_ptr < SerializableSet < string >> miners = ledger.getObjectOrNull < SerializableSet < string >>( Ledger::keyFor_miners ());
    assert ( miners );
    
    set < string >::const_iterator minerIt = miners->cbegin ();
    for ( ; minerIt != miners->cend (); ++minerIt ) {
        minerInfoSet.insert ( *minerIt );
    }
    return minerInfoSet;
}

//----------------------------------------------------------------//
LedgerResult Ledger_Miner::registerMiner ( AccountID accountID, const MinerInfo& minerInfo ) {

    if ( !minerInfo.isValid ()) return "Invalid miner info.";

    AbstractLedger& ledger = this->getLedger ();
    
    AccountODBM accountODBM ( ledger, accountID );
    if ( !accountODBM ) return "Miner account not found.";
    
    accountODBM.mMinerInfo.set ( minerInfo );
    accountODBM.mMinerHeight.set ( ledger.countBlocks ());
    
    // TODO: find an efficient way to do all this
    string accountName = accountODBM.mName.get ();
    
    LedgerKey KEY_FOR_MINERS = Ledger::keyFor_miners ();
    shared_ptr < SerializableSet < string >> miners = ledger.getObjectOrNull < SerializableSet < string >>( KEY_FOR_MINERS );
    miners = miners ? miners : make_shared < SerializableSet < string >>();
    miners->insert ( accountName );
    ledger.setObject < SerializableSet < string >>( KEY_FOR_MINERS, *miners );

    return true;
}

} // namespace Volition
