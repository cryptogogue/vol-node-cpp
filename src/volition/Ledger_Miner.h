// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_MINER_H
#define VOLITION_LEDGER_MINER_H

#include <volition/common.h>
#include <volition/AbstractLedgerComponent.h>
#include <volition/Account.h>
#include <volition/AccountEntitlements.h>
#include <volition/AccountKeyLookup.h>
#include <volition/Asset.h>
#include <volition/LedgerResult.h>
#include <volition/MinerInfo.h>

namespace Volition {

//================================================================//
// Ledger_Miner
//================================================================//
class Ledger_Miner :
    virtual public AbstractLedgerComponent {
public:

    typedef SerializableMap < string, string > MinerURLMap;

    //----------------------------------------------------------------//
    set < string >                      getMiners                       () const;
    LedgerResult                        registerMiner                   ( AccountID accountID, const MinerInfo& minerInfo, bool requireFingerprint );
    LedgerResult                        updateMinerInfo                 ( AccountID accountID, const MinerInfo& minerInfo );
};

} // namespace Volition
#endif
