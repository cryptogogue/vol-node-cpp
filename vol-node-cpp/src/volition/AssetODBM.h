// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETODBM_H
#define VOLITION_ASSETODBM_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/Asset.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LedgerKey.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// AssetODBM
//================================================================//
class AssetODBM {
public:

    ConstOpt < Ledger >     mLedger;
    AssetID::Index          mIndex;

    LedgerFieldODBM < AssetID::Index >  mOwner;
    LedgerFieldODBM < size_t >          mInventoryNonce;
    LedgerFieldODBM < size_t >          mPosition;
    LedgerFieldODBM < string >          mType;

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryNonce ( AssetID::Index index ) {
        return Format::write ( "asset.%d.inventoryNonce", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_owner ( AssetID::Index index ) {
        return Format::write ( "asset.%d.owner", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_position ( AssetID::Index index ) {
        return Format::write ( "asset.%d.position", index );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_type ( AssetID::Index index ) {
        return Format::write ( "asset.%d.type", index );
    }

    //----------------------------------------------------------------//
    AssetODBM ( ConstOpt < Ledger > ledger, AssetID::Index index ) :
        mLedger ( ledger ),
        mIndex ( index ),
        mOwner ( ledger,            keyFor_owner ( this->mIndex )),
        mInventoryNonce ( ledger,   keyFor_inventoryNonce ( this->mIndex )),
        mPosition ( ledger,         keyFor_position ( this->mIndex )),
        mType ( ledger,             keyFor_type ( this->mIndex )) {
    }
};

} // namespace Volition
#endif
