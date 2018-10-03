// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZERTO_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZERTO_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/AbstractStringifiable.h>
#include <volition/serialization/SerializerPropertyName.h>

namespace Volition {

//================================================================//
// AbstractSerializerTo
//================================================================//
class AbstractSerializerTo {
protected:

    //----------------------------------------------------------------//
    virtual bool        AbstractSerializerTo_isDigest       () = 0;
    virtual void        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const u64& value ) = 0;
    virtual void        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const string& value ) = 0;
    virtual void        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const AbstractSerializable& value ) = 0;
    virtual void        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const AbstractSerializableArray& value ) = 0;
    virtual void        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const AbstractSerializablePointer& value ) = 0;
    virtual void        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const AbstractStringifiable& value ) = 0;

public:
    
    //----------------------------------------------------------------//
    bool isDigest () {
        return this->AbstractSerializerTo_isDigest ();
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void serialize ( SerializerPropertyName name, const TYPE& value ) {
        this->AbstractSerializerTo_serialize ( name, value );
    }
};

} // namespace Volition
#endif
