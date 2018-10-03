// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZERFROM_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZERFROM_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/AbstractStringifiable.h>
#include <volition/serialization/SerializerPropertyName.h>

namespace Volition {

//================================================================//
// AbstractSerializerFrom
//================================================================//
class AbstractSerializerFrom {
public:

    enum Mode {
        SERIALIZE_DIGEST,
        SERIALIZE_IN,
        SERIALIZE_OUT,
    };

protected:

    //----------------------------------------------------------------//
    virtual size_t      AbstractSerializerFrom_getSize          () = 0;
    virtual bool        AbstractSerializerFrom_has              ( SerializerPropertyName name ) const = 0;
    virtual void        AbstractSerializerFrom_serialize        ( SerializerPropertyName name, u64& value ) const = 0;
    virtual void        AbstractSerializerFrom_serialize        ( SerializerPropertyName name, string& value ) const = 0;
    virtual void        AbstractSerializerFrom_serialize        ( SerializerPropertyName name, AbstractSerializable& value ) const = 0;
    virtual void        AbstractSerializerFrom_serialize        ( SerializerPropertyName name, AbstractSerializableArray& value ) const = 0;
    virtual void        AbstractSerializerFrom_serialize        ( SerializerPropertyName name, AbstractSerializablePointer& value ) const = 0;
    virtual void        AbstractSerializerFrom_serialize        ( SerializerPropertyName name, AbstractStringifiable& value ) const = 0;

public:
    
    //----------------------------------------------------------------//
    size_t getSize () {
        return this->AbstractSerializerFrom_getSize ();
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void serialize ( SerializerPropertyName name, TYPE& value ) const {
        this->AbstractSerializerFrom_serialize ( name, value );
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    TYPE serializeIn ( SerializerPropertyName name, const TYPE& fallback ) const {
        TYPE value = fallback;
        this->AbstractSerializerFrom_serialize ( name, value );
        return value;
    }
};

} // namespace Volition
#endif
