// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLETIME_H
#define VOLITION_SERIALIZATION_SERIALIZABLETIME_H

#include <volition/Format.h>
#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// SerializableTime
//================================================================//
class SerializableTime :
    public AbstractSerializable {
public:
    
    time_t      mTime;
    
    //----------------------------------------------------------------//
    operator time_t () const {
    
        return this->mTime;
    }
    
    //----------------------------------------------------------------//
    operator string () const {
    
        return Format::toISO8601 ( this->mTime );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        const AbstractSerializerFrom* parent = serializer.getParent ();
        if ( parent ) {
            string iso8601;
            parent->serialize ( serializer.getName (), iso8601 );
            this->mTime = Format::fromISO8601 ( iso8601 );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        
        AbstractSerializerTo* parent = serializer.getParent ();
        if ( parent ) {
            if ( serializer.isDigest ()) {
                u64 t = ( u64 )this->mTime;
                parent->serialize ( serializer.getName (), t );
            }
            else {
                string iso8601 = Format::toISO8601 ( this->mTime );
                parent->serialize ( serializer.getName (), iso8601 );
            }
        }
    }
    
    //----------------------------------------------------------------//
    SerializableTime () {
    }
    
    //----------------------------------------------------------------//
    SerializableTime ( time_t t ) :
        mTime ( t ) {
    }
};

} // namespace Volition
#endif
