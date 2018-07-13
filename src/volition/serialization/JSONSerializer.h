// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_JSONSERIALIZER_H
#define VOLITION_SERIALIZATION_JSONSERIALIZER_H

#include <volition/serialization/AbstractSerializer.h>

namespace Volition {

//================================================================//
// JSONSerializableTypeInfo
//================================================================//
class JSONSerializableTypeInfo :
    public AbstractSerializableTypeInfo {
protected:

    const Poco::JSON::Object&   mObject;

    //----------------------------------------------------------------//
    string AbstractSerializableTypeInfo_get ( string key ) const {
        return this->mObject.optValue < string >( key, "" );
    }

public:

    //----------------------------------------------------------------//
    JSONSerializableTypeInfo ( const Poco::JSON::Object& object ) :
        mObject ( object ) {
    }
};

//================================================================//
// JSONSerializer
//================================================================//
template < typename ARRAY_TYPE, typename OBJECT_TYPE >
class JSONSerializer :
    public AbstractSerializer {
protected:
    
    ARRAY_TYPE*     mArray;
    OBJECT_TYPE*    mObject;

    //----------------------------------------------------------------//
    JSONSerializer () :
        mObject ( NULL ),
        mArray ( NULL ) {
    }

    //----------------------------------------------------------------//
    Poco::JSON::Array::Ptr getArray ( SerializerPropertyName name ) {
    
        if ( this->mArray ) {
            return this->mArray->getArray (( unsigned int )name.getIndex ());
        }
        return this->mObject->getArray ( name.getName ());
    }

    //----------------------------------------------------------------//
    Poco::JSON::Object::Ptr getObject ( SerializerPropertyName name ) {
    
        if ( this->mArray ) {
            return this->mArray->getObject (( unsigned int )name.getIndex ());
        }
        return this->mObject->getObject ( name.getName ());
    }

    //----------------------------------------------------------------//
    bool has ( SerializerPropertyName name ) {
    
        if ( this->mArray ) {
            return name.getIndex () < this->mArray->size ();
        }
        return this->mObject->has ( name.getName ());
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    TYPE optValue ( SerializerPropertyName name, const TYPE& fallback ) {
    
        assert ( this->mObject || this->mArray );
    
        if ( this->mArray ) {
            return this->mArray->optElement (( unsigned int )name.getIndex (), fallback );
        }
        return this->mObject->optValue ( name.getName (), fallback );
    }

    //----------------------------------------------------------------//
    void set ( SerializerPropertyName name, const Poco::Dynamic::Var& value ) {
    
        assert ( this->mObject || this->mArray );
    
        if ( this->mArray ) {
            this->mArray->set (( unsigned int )name.getIndex (), value );
        }
        else {
            this->mObject->set ( name.getName (), value );
        }
    }
};

} // namespace Volition
#endif
