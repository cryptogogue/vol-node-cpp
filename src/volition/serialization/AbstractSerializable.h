// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLE_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZABLE_H

#include <volition/common.h>

namespace Volition {

class AbstractSerializerFrom;
class AbstractSerializerTo;

//================================================================//
// AbstractSerializable
//================================================================//
class AbstractSerializable {
protected:

    //----------------------------------------------------------------//
    virtual void        AbstractSerializable_serializeFrom      () {};
    virtual void        AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) = 0;
    virtual void        AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const = 0;

public:

    //----------------------------------------------------------------//
    AbstractSerializable () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractSerializable () {
    }

    //----------------------------------------------------------------//
    void serializeFrom () {
        this->AbstractSerializable_serializeFrom ();
    }

    //----------------------------------------------------------------//
    void serializeFrom ( const AbstractSerializerFrom& serializer ) {
        this->AbstractSerializable_serializeFrom ( serializer );
    }
    
    //----------------------------------------------------------------//
    void serializeTo ( AbstractSerializerTo& serializer ) const {
        this->AbstractSerializable_serializeTo ( serializer );
    }
};

} // namespace Volition
#endif
