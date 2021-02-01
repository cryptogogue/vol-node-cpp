// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/common.h>
#include <volition/AssetODBM.h>
#include <volition/InventoryLogEntry.h>
#include <volition/Ledger.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// InventoryLogEntry
//================================================================//
    
//----------------------------------------------------------------//
void InventoryLogEntry::apply ( SerializableSet < AssetID::Index >& additions, SerializableSet < AssetID::Index >& deletions ) {
    
    SerializableSet < AssetID::Index >::const_iterator assetIt;
    
    InventoryLogEntry::applyToSet ( this->mAdditions, additions );
    InventoryLogEntry::applyToSet ( this->mDeletions, deletions );
}

//----------------------------------------------------------------//
void InventoryLogEntry::applyToSet ( SerializableSet < AssetID::Index > assets, SerializableSet < AssetID::Index >& set ) {
            
    SerializableSet < AssetID::Index >::const_iterator assetIt = assets.cbegin ();
    for ( ; assetIt != assets.cend (); ++assetIt ) {
        set.insert ( *assetIt );
    }
}
    
////----------------------------------------------------------------//
//void InventoryLogEntry::apply ( SerializableSet < AssetID::Index > assets, SerializableSet < AssetID::Index >& positive, SerializableSet < AssetID::Index >& negative ) {
//
//    SerializableSet < AssetID::Index >::const_iterator assetIt = assets.cbegin ();
//    for ( ; assetIt != assets.cend (); ++assetIt ) {
//        AssetID::Index assetID = *assetIt;
//        if ( negative.find ( assetID ) != negative.cend ()) {
//            negative.erase ( assetID );
//        }
//        else {
//            positive.insert ( assetID );
//        }
//    }
//}
    
//----------------------------------------------------------------//
void InventoryLogEntry::decode ( const SerializableSet < AssetID::Index >& indexSet, SerializableList < string >& assetIDs ) {
    
    SerializableSet < AssetID::Index >::const_iterator indexIt = indexSet.cbegin ();
    for ( ; indexIt != indexSet.cend (); ++indexIt ) {
        assetIDs.push_back ( AssetID::encode ( *indexIt ));
    }
}

//----------------------------------------------------------------//
void InventoryLogEntry::expand ( const Ledger& ledger, const Schema& schema, string accountName, const SerializableSet < AssetID::Index >& indexSet, SerializableList < SerializableSharedPtr < Asset >>& assetList ) {
    
    Account::Index accountID = ledger.getAccountIndex ( accountName );
    
    SerializableSet < AssetID::Index >::const_iterator indexIt = indexSet.cbegin ();
    for ( ; indexIt != indexSet.cend (); ++indexIt ) {
    
        AssetID::Index assetID = *indexIt;
        Account::Index ownerID = LedgerFieldODBM < AssetID::Index >( ledger, AssetODBM::keyFor_owner ( assetID )).get ( Account::NULL_INDEX );
    
        if ( ownerID == accountID ) {
            shared_ptr < Asset > asset = ledger.getAsset ( schema, assetID, true );
            assert ( asset );
            assetList.push_back ( asset );
        }
    }
}

//----------------------------------------------------------------//
void InventoryLogEntry::insert ( AssetID::Index assetID, EntryOp op ) {

    switch ( op ) {
    
        case ADD_ASSET:
            this->insertAddition ( assetID );
            break;
            
        case DELETE_ASSET:
            this->insertDeletion ( assetID );
            break;
            
        case UPDATE_ASSET:
            this->insertUpdate ( assetID );
            break;
    }
}

//----------------------------------------------------------------//
void InventoryLogEntry::insertAddition ( AssetID::Index addition ) {
    this->mAdditions.insert ( addition );
}

//----------------------------------------------------------------//
void InventoryLogEntry::insertDeletion ( AssetID::Index deletion ) {
    this->mDeletions.insert ( deletion );
}

//----------------------------------------------------------------//
void InventoryLogEntry::insertUpdate ( AssetID::Index update ) {
    this->insertDeletion ( update );
    this->insertAddition ( update );
}

//----------------------------------------------------------------//
InventoryLogEntry::InventoryLogEntry () :
    mTime ( 0 ) {
}

//----------------------------------------------------------------//
InventoryLogEntry::InventoryLogEntry ( time_t time ) :
    mTime ( time ) {
}

} // namespace Volition
