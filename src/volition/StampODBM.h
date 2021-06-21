// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_STAMPODBM_H
#define VOLITION_STAMPODBM_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LedgerKey.h>
#include <volition/LedgerObjectFieldODBM.h>
#include <volition/Stamp.h>

namespace Volition {

//================================================================//
// StampODBM
//================================================================//
class StampODBM {
private:

    //----------------------------------------------------------------//
    static LedgerKey keyFor_body ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "stamp.%d.body", index ); });
    }

     //----------------------------------------------------------------//
    static LedgerKey keyFor_price ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "stamp.%d.price", index ); });
    }

public:

    ConstOpt < AbstractLedger >             mLedger;
    AssetID                                 mAssetID;

    LedgerFieldODBM < u64 >                 mPrice; // track the price here in case user wants to change it (don't need to rewrite fields)
    LedgerObjectFieldODBM < Stamp >         mBody;

    //----------------------------------------------------------------//
    operator bool () {
        return this->mBody.exists ();
    }

    //----------------------------------------------------------------//
    StampODBM ( ConstOpt < AbstractLedger > ledger, AssetID::Index index ) :
        mLedger ( ledger ),
        mAssetID ( index ),
        mPrice ( ledger,            keyFor_price ( this->mAssetID ),            0 ),
        mBody ( ledger,             keyFor_body ( this->mAssetID )) {
    }
};

} // namespace Volition
#endif
