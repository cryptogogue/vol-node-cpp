// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_CHAINMETADATA_H
#define VOLITION_CHAINMETADATA_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/SerializableSet.h>
#include <volition/serialization/SerializableVector.h>

namespace Volition {

//================================================================//
// CycleMetadata
//================================================================//
class CycleMetadata :
    public AbstractSerializable {
private:

    friend class Chain;
    friend class ChainMetadata;

    SerializableSet < string > mKnownParticipants;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        serializer.serialize ( "participants", this->mKnownParticipants );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.serialize ( "participants", this->mKnownParticipants );
    }

public:
};

//================================================================//
// ChainMetadata
//================================================================//
class ChainMetadata :
    public AbstractSerializable {
private:

    friend class Chain;

    SerializableVector < CycleMetadata > mCycleMetadata;

    //----------------------------------------------------------------//
    void        AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void        AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    void        affirmCycle             ( size_t cycleID );
    void        affirmParticipant       ( size_t cycleID, string participant );
    bool        canEdit                 ( size_t cycleID, string minerID = "" ) const;
                ChainMetadata           ();
                ~ChainMetadata          ();
    size_t      countParticipants       ( size_t cycleID, string minerID = "" ) const;
    bool        isParticipant           ( size_t cycleID, string minerID ) const;
};

} // namespace Volition
#endif
