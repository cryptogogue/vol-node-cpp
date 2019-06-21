// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZERFROM_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZERFROM_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/SerializerKeys.h>
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
    
    enum KeyType {
        KEY_TYPE_INDEX,
        KEY_TYPE_STRING,
        KEY_TYPE_NONE,
    };

protected:

    typedef std::function < void ( const AbstractSerializerFrom& )> SerializationFunc;

    //----------------------------------------------------------------//
    virtual SerializerKeys                  AbstractSerializerFrom_getKeys          () const = 0;
    virtual KeyType                         AbstractSerializerFrom_getKeyType       () const = 0;
    virtual SerializerPropertyName          AbstractSerializerFrom_getName          () const = 0;
    virtual const AbstractSerializerFrom*   AbstractSerializerFrom_getParent        () const = 0;
    virtual size_t                          AbstractSerializerFrom_getSize          () const = 0;
    virtual bool                            AbstractSerializerFrom_has              ( SerializerPropertyName name ) const = 0;
    virtual void                            AbstractSerializerFrom_serialize        ( SerializerPropertyName name, bool& value ) const = 0;
    virtual void                            AbstractSerializerFrom_serialize        ( SerializerPropertyName name, double& value ) const = 0;
    virtual void                            AbstractSerializerFrom_serialize        ( SerializerPropertyName name, u64& value ) const = 0;
    virtual void                            AbstractSerializerFrom_serialize        ( SerializerPropertyName name, string& value ) const = 0;
    virtual void                            AbstractSerializerFrom_serialize        ( SerializerPropertyName name, AbstractSerializable& value ) const = 0;
    virtual void                            AbstractSerializerFrom_serialize        ( SerializerPropertyName name, const SerializationFunc& serializeFunc ) const = 0;
    virtual void                            AbstractSerializerFrom_stringFromTree   ( SerializerPropertyName name, string& value ) const = 0;

public:
    
    //----------------------------------------------------------------//
    void context ( SerializerPropertyName name, const SerializationFunc& serializeFunc ) const {
        this->AbstractSerializerFrom_serialize ( name, serializeFunc );
    }
    
    //----------------------------------------------------------------//
    SerializerKeys getKeys () const {
        return this->AbstractSerializerFrom_getKeys ();
    }
    
    //----------------------------------------------------------------//
    KeyType getKeyType () const {
        return this->AbstractSerializerFrom_getKeyType ();
    }
    
    //----------------------------------------------------------------//
    SerializerPropertyName getName () const {
        return AbstractSerializerFrom_getName ();
    }
    
    //----------------------------------------------------------------//
    const AbstractSerializerFrom* getParent () const {
        return AbstractSerializerFrom_getParent ();
    }
    
    //----------------------------------------------------------------//
    size_t getSize () const {
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
    
    //----------------------------------------------------------------//
    void stringFromTree ( SerializerPropertyName name, string& value ) const {
        this->AbstractSerializerFrom_stringFromTree ( name, value );
    }
};

} // namespace Volition
#endif
