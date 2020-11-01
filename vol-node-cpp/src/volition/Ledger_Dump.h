// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_LEDGER_DUMP_H
#define VOLITION_LEDGER_DUMP_H

#include <volition/common.h>
#include <volition/AbstractLedgerComponent.h>
#include <volition/Asset.h>
#include <volition/InventoryLogEntry.h>
#include <volition/LedgerResult.h>

namespace Volition {

class AccountODBM;
class AssetODBM;
class Schema;

//================================================================//
// Ledger_Dump
//================================================================//
class Ledger_Dump :
    virtual public AbstractLedgerComponent {
public:

    //----------------------------------------------------------------//
    void                                dump                        ( string sqlfile );
};

} // namespace Volition
#endif
