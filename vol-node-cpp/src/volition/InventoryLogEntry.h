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
    void decodeAdditions ( SerializableList < string >& assetIDs ) const {
        
        SerializableSet < AssetID::Index >::const_iterator additionIt = this->mAdditions.cbegin ();
        for ( ; additionIt != this->mAdditions.cend (); ++additionIt ) {
            assetIDs.push_back ( AssetID::encode ( *additionIt ));
        }
    }
    
    //----------------------------------------------------------------//
    void decodeDeletions ( SerializableList < string >& assetIDs ) const {
        
        SerializableSet < AssetID::Index >::const_iterator deletionIt = this->mDeletions.cbegin ();
        for ( ; deletionIt != this->mDeletions.cend (); ++deletionIt ) {
            assetIDs.push_back ( AssetID::encode ( *deletionIt ));
        }
    }
    
    //----------------------------------------------------------------//
    void expandAdditions ( const Ledger& ledger, const Schema& schema, string accountName, SerializableList < SerializableSharedPtr < Asset >>& assetList ) const {
        
        Account::Index accountID = ledger.getAccountIndex ( accountName );
        
        SerializableSet < AssetID::Index >::const_iterator additionIt = this->mAdditions.cbegin ();
        for ( ; additionIt != this->mAdditions.cend (); ++additionIt ) {
        
            AssetID::Index assetID = *additionIt;
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
