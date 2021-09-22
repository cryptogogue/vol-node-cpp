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
    static LedgerKey keyFor_available ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "stamp.%d.available", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_body ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "stamp.%d.body", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_price ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "stamp.%d.price", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_version ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "stamp.%d.version", index ); });
    }

public:

    ConstOpt < AbstractLedger >             mLedger;
    AssetID                                 mAssetID;

    LedgerFieldODBM < u64 >                 mPrice; // track the price here in case user wants to change it (don't need to rewrite fields)
    LedgerFieldODBM < u64 >                 mVersion; // changed any time the stamp is updated
    LedgerFieldODBM < bool >                mAvailable;
    LedgerObjectFieldODBM < Stamp >         mBody;

    //----------------------------------------------------------------//
    operator bool () {
        return this->mBody.exists () && ( this->mAvailable.get () == true );
    }

    //----------------------------------------------------------------//
    StampODBM ( ConstOpt < AbstractLedger > ledger, AssetID::Index index ) :
        mLedger         ( ledger ),
        mAssetID        ( index ),
        mPrice          ( ledger,       keyFor_price ( this->mAssetID ),            0 ),
        mVersion        ( ledger,       keyFor_version ( this->mAssetID ),          0 ),
        mAvailable      ( ledger,       keyFor_available ( this->mAssetID ),        true ),
        mBody           ( ledger,       keyFor_body ( this->mAssetID )) {
    }
};

} // namespace Volition
#endif
