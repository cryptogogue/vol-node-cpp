// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_OFFERODBM_H
#define VOLITION_OFFERODBM_H

#include <volition/common.h>
#include <volition/IndexID.h>
#include <volition/KeyAndPolicy.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// OfferODBM
//================================================================//
class OfferODBM {
private:

    //----------------------------------------------------------------//
    static LedgerKey keyFor_assetIdentifiers ( OfferID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "offer.%d.assetIdentifiers", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_expiration ( OfferID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "offer.%d.expiration", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_minimumPrice ( OfferID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "offer.%d.minimumPrice", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_position ( OfferID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "offer.%d.position", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_seller ( OfferID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "offer.%d.seller", index ); });
    }

public:

    //----------------------------------------------------------------//
    static LedgerKey keyFor_globalOfferCount () {
        return Format::write ( "offer.count" );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_globalOpenOfferCount () {
        return Format::write ( "offer.openCount" );
    }

    ConstOpt < AbstractLedger >             mLedger;
    OfferID                                 mOfferID;
    LedgerFieldODBM < AccountID::Index >    mSeller;
    LedgerFieldODBM < u64 >                 mMinimumPrice;
    LedgerFieldODBM < string >              mExpiration;
    LedgerFieldODBM < u64 >                 mPosition; // position in active offer list

    LedgerObjectFieldODBM < SerializableVector < AssetID::Index >> mAssetIdentifiers;

    //----------------------------------------------------------------//
    operator bool () {
        return this->mSeller.exists ();
    }

    //----------------------------------------------------------------//
    OfferODBM ( ConstOpt < AbstractLedger > ledger, OfferID::Index index ) :
        mLedger ( ledger ),
        mOfferID ( index ),
        mSeller ( ledger,               keyFor_seller ( this->mOfferID ),               OfferID::NULL_INDEX ),
        mMinimumPrice ( ledger,         keyFor_minimumPrice ( this->mOfferID ),         0 ),
        mExpiration ( ledger,           keyFor_expiration ( this->mOfferID ),           "" ),
        mPosition ( ledger,             keyFor_position ( this->mOfferID ),             0 ),
        mAssetIdentifiers ( ledger,     keyFor_assetIdentifiers ( this->mOfferID )) {
    }
};

} // namespace Volition
#endif
