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

    typedef std::function < void ( AbstractSerializerTo& )> SerializationFunc;

    //----------------------------------------------------------------//
    virtual void                        AbstractSerializerTo_affirmArray        () = 0;
    virtual void                        AbstractSerializerTo_affirmObject       () = 0;
    virtual SerializerPropertyName      AbstractSerializerTo_getName            () const = 0;
    virtual AbstractSerializerTo*       AbstractSerializerTo_getParent          () = 0;
    virtual bool                        AbstractSerializerTo_isDigest           () const = 0;
    virtual void                        AbstractSerializerTo_serialize          ( SerializerPropertyName name, const bool& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize          ( SerializerPropertyName name, const double& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize          ( SerializerPropertyName name, const u64& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize          ( SerializerPropertyName name, const string& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize          ( SerializerPropertyName name, const AbstractSerializable& value ) = 0;
    virtual void                        AbstractSerializerTo_serialize          ( SerializerPropertyName name, const SerializationFunc& serializeFunc ) = 0;
    virtual void                        AbstractSerializerFrom_stringToTree     ( SerializerPropertyName name, string value ) = 0;

public:
    
    //----------------------------------------------------------------//
    void affirmArray () {
        this->AbstractSerializerTo_affirmArray ();
    }
    
    //----------------------------------------------------------------//
    void affirmObject () {
        this->AbstractSerializerTo_affirmObject ();
    }
    
    //----------------------------------------------------------------//
    void context ( SerializerPropertyName name, const SerializationFunc& serializeFunc ) {
        this->AbstractSerializerTo_serialize ( name, serializeFunc );
    }
    
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
    void serialize ( SerializerPropertyName name, const char* value ) {
        this->serialize ( name, string ( value ));
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void serialize ( SerializerPropertyName name, const TYPE& value ) {
        this->AbstractSerializerTo_serialize ( name, value );
    }

    //----------------------------------------------------------------//
    void stringToTree ( SerializerPropertyName name, string value ) {
        this->AbstractSerializerFrom_stringToTree ( name, value );
    }
};

} // namespace Volition
#endif
