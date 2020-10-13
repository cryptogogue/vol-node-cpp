// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_INVENTORYLOGENTRY_H
#define VOLITION_INVENTORYLOGENTRY_H

#include <volition/common.h>
#include <volition/AssetID.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

class AssetODBM;
class Schema;

//================================================================//
// InventoryLogEntry
//================================================================//
class InventoryLogEntry :
    public AbstractSerializable {
public:

    enum EntryOp {
        ADD_ASSET,
        DELETE_ASSET,
        UPDATE_ASSET,
    };

    SerializableSet < AssetID::Index >      mAdditions;
    SerializableSet < AssetID::Index >      mDeletions;
    SerializableTime                        mTime;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "additions",         this->mAdditions );
        serializer.serialize ( "deletions",         this->mDeletions );
        serializer.serialize ( "time",              this->mTime );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "additions",         this->mAdditions );
        serializer.serialize ( "deletions",         this->mDeletions );
        serializer.serialize ( "time",              this->mTime );
    }
    
    //----------------------------------------------------------------//
    void            apply                   ( SerializableSet < AssetID::Index >& additions, SerializableSet < AssetID::Index >& deletions ) const;
    static void     applyToSet              ( const SerializableSet < AssetID::Index >& assets, SerializableSet < AssetID::Index >& set );
    static void     decode                  ( const SerializableSet < AssetID::Index >& indexSet, SerializableList < string >& assetIDs );
    static void     expand                  ( const Ledger& ledger, const Schema& schema, string accountName, const SerializableSet < AssetID::Index >& indexSet, SerializableList < SerializableSharedConstPtr < Asset >>& assetList );
    void            insert                  ( AssetID::Index assetID, EntryOp op );
    void            insertAddition          ( AssetID::Index addition );
    void            insertDeletion          ( AssetID::Index deletion );
    void            insertUpdate            ( AssetID::Index update );
                    InventoryLogEntry       ();
                    InventoryLogEntry       ( time_t time );
};

} // namespace Volition
#endif
