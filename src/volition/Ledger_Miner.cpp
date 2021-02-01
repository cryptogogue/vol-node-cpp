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
shared_ptr < MinerInfo > Ledger_Miner::getMinerInfo ( Account::Index accountIndex ) const {

    return this->getLedger ().getObjectOrNull < MinerInfo >( AccountODBM::keyFor_minerInfo ( accountIndex ));
}

//----------------------------------------------------------------//
map < string, MinerInfo > Ledger_Miner::getMiners () const {

    const Ledger& ledger = this->getLedger ();

    map < string, MinerInfo > minerInfoMap;

    shared_ptr < SerializableSet < string >> miners = ledger.getObjectOrNull < SerializableSet < string >>( Ledger::keyFor_miners ());
    assert ( miners );
    
    set < string >::const_iterator minerIt = miners->cbegin ();
    for ( ; minerIt != miners->cend (); ++minerIt ) {
    
        const string& minerID = *minerIt;
        
        shared_ptr < MinerInfo > minerInfo = ledger.getMinerInfo ( ledger.getAccountIndex ( minerID ));
        assert ( minerInfo );
        minerInfoMap [ minerID ] = *minerInfo;
    }
    return minerInfoMap;
}

//----------------------------------------------------------------//
shared_ptr < Ledger::MinerURLMap > Ledger_Miner::getMinerURLs () const {

    const Ledger& ledger = this->getLedger ();
    
    return ledger.getObjectOrNull < MinerURLMap >( Ledger::keyFor_minerURLs ());
}

//----------------------------------------------------------------//
bool Ledger_Miner::registerMiner ( Account::Index accountIndex, string keyName, string url ) {

    Ledger& ledger = this->getLedger ();

    AccountKey accountKey = ledger.getAccountKey ( accountIndex, keyName );
    if ( accountKey ) {

        shared_ptr < Account > account = accountKey.mAccount;

        ledger.setObject < MinerInfo >(
            AccountODBM::keyFor_minerInfo ( accountIndex ),
            MinerInfo ( accountIndex, url, accountKey.mKeyAndPolicy->mKey )
        );
        
        // TODO: find an efficient way to do all this
        string accountName = ledger.getAccountName ( accountIndex );
        
        LedgerKey KEY_FOR_MINERS = Ledger::keyFor_miners ();
        shared_ptr < SerializableSet < string >> miners = ledger.getObjectOrNull < SerializableSet < string >>( KEY_FOR_MINERS );
        assert ( miners );
        miners->insert ( accountName );
        ledger.setObject < SerializableSet < string >>( KEY_FOR_MINERS, *miners );
        
        LedgerKey KEY_FOR_MINER_URLS = Ledger::keyFor_minerURLs ();
        shared_ptr < SerializableMap < string, string >> minerURLs = ledger.getObjectOrNull < SerializableMap < string, string >>( KEY_FOR_MINER_URLS );
        assert ( minerURLs );
        ( *minerURLs )[ accountName ] = url;
        ledger.setObject < SerializableMap < string, string >>( KEY_FOR_MINER_URLS, *minerURLs );

        return true;
    }
    return false;
}

} // namespace Volition
