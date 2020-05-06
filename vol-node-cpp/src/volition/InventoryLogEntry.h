// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_INVENTORYLOGENTRY_H
#define VOLITION_INVENTORYLOGENTRY_H

#include <volition/common.h>
#include <volition/AssetODBM.h>
#include <volition/KeyAndPolicy.h>
#include <volition/Ledger.h>
#include <volition/Schema.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// Account
//================================================================//
class InventoryLogEntry :
    public AbstractSerializable {
public:

    SerializableSet < AssetID::Index >      mAdditions;
    SerializableSet < AssetID::Index >      mDeletions;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "additions",         this->mAdditions );
        serializer.serialize ( "deletions",         this->mDeletions );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "additions",         this->mAdditions );
        serializer.serialize ( "deletions",         this->mDeletions );
    }
    
    //----------------------------------------------------------------//
    void apply ( SerializableSet < AssetID::Index >& additions, SerializableSet < AssetID::Index >& deletions ) {
        
        SerializableSet < AssetID::Index >::const_iterator assetIt;
        
        InventoryLogEntry::apply ( this->mAdditions, additions, deletions );
        InventoryLogEntry::apply ( this->mDeletions, deletions, additions );
    }
    
    //----------------------------------------------------------------//
    static void apply ( SerializableSet < AssetID::Index > assets, SerializableSet < AssetID::Index >& positive, SerializableSet < AssetID::Index >& negative ) {
                
        SerializableSet < AssetID::Index >::const_iterator assetIt = assets.cbegin ();
        for ( ; assetIt != assets.cend (); ++assetIt ) {
            AssetID::Index assetID = *assetIt;
            if ( negative.find ( assetID ) != negative.cend ()) {
                negative.erase ( assetID );
            }
            else {
                positive.insert ( assetID );
            }
        }
    }
    
    //----------------------------------------------------------------//
    static void decode ( const SerializableSet < AssetID::Index >& indexSet, SerializableList < string >& assetIDs ) {
        
        SerializableSet < AssetID::Index >::const_iterator indexIt = indexSet.cbegin ();
        for ( ; indexIt != indexSet.cend (); ++indexIt ) {
            assetIDs.push_back ( AssetID::encode ( *indexIt ));
        }
    }
    
    //----------------------------------------------------------------//
    static void expand ( const Ledger& ledger, const Schema& schema, string accountName, const SerializableSet < AssetID::Index >& indexSet, SerializableList < SerializableSharedPtr < Asset >>& assetList ) {
        
        Account::Index accountID = ledger.getAccountIndex ( accountName );
        
        SerializableSet < AssetID::Index >::const_iterator indexIt = indexSet.cbegin ();
        for ( ; indexIt != indexSet.cend (); ++indexIt ) {
        
            AssetID::Index assetID = *indexIt;
            Account::Index ownerID = LedgerFieldODBM < AssetID::Index >( ledger, AssetODBM::keyFor_owner ( assetID )).get ( Account::NULL_INDEX );
        
            if ( ownerID == accountID ) {
                shared_ptr < Asset > asset = ledger.getAsset ( schema, assetID );
                assert ( asset );
                assetList.push_back ( asset );
            }
        }
    }
    
    //----------------------------------------------------------------//
    void insertAddition ( AssetID::Index addition ) {
        this->mAdditions.insert ( addition );
    }

    //----------------------------------------------------------------//
    void insertDeletion ( AssetID::Index deletion ) {
        this->mDeletions.insert ( deletion );
    }
};

} // namespace Volition
#endif
