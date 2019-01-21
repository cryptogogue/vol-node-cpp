// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZERTO_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZERTO_H

#include <volition/common.h>
#include <volition/serialization/AbstractSerializable.h>
#include <volition/serialization/SerializerPropertyName.h>

namespace Volition {

//================================================================//
// AbstractSerializerTo
//================================================================//
class AbstractSerializerTo {
protected:

    //----------------------------------------------------------------//
    virtual SerializerPropertyName      AbstractSerializerTo_getName        () const = 0;
    virtual AbstractSerializerTo*       AbstractSerializerTo_getParent      () = 0;
    virtual bool                        AbstractSerializerTo_isDigest       () const = 0;
    virtual void                        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const bool& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const double& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const u64& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const string& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const AbstractSerializable& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize      ( SerializerPropertyName name, const Variant& value ) = 0;

public:
    
    //----------------------------------------------------------------//
    SerializerPropertyName getName () const {
        return AbstractSerializerTo_getName ();
    }
    
    //----------------------------------------------------------------//
    AbstractSerializerTo* getParent () {
        return AbstractSerializerTo_getParent ();
    }
    
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
