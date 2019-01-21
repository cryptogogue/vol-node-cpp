// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_JSONSERIALIZER_H
#define VOLITION_SERIALIZATION_JSONSERIALIZER_H

#include <volition/serialization/Variant.h>

namespace Volition {

//================================================================//
// JSONSerializer
//================================================================//
template < typename ARRAY_TYPE, typename OBJECT_TYPE, typename PARENT_TYPE >
class JSONSerializer {
protected:
    
    ARRAY_TYPE*             mArray;
    OBJECT_TYPE*            mObject;
    PARENT_TYPE*            mParent;
    SerializerPropertyName  mName;

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mArray || this->mObject );
    }
    
    //----------------------------------------------------------------//
    operator Poco::Dynamic::Var () const {
        if ( this->mArray ) return this->mArray;
        return this->mObject;
    }

    //----------------------------------------------------------------//
    JSONSerializer () :
        mObject ( NULL ),
        mArray ( NULL ),
        mParent ( NULL ) {
    }

    //----------------------------------------------------------------//
    const Poco::Dynamic::Var get ( SerializerPropertyName name ) const {
    
        if ( this->mArray ) {
            return this->mArray->get (( unsigned int )name.getIndex ());
        }
        return this->mObject->get ( name.getName ());
    }

    //----------------------------------------------------------------//
    bool has ( SerializerPropertyName name ) const {
    
        if ( this->mArray ) {
            return name.getIndex () < this->mArray->size ();
        }
        return this->mObject->has ( name.getName ());
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    TYPE optValue ( SerializerPropertyName name, const TYPE& fallback ) const {
    
        assert ( this->mObject || this->mArray );
    
        Poco::Dynamic::Var value;
    
        if ( this->mArray ) {
            value = this->mArray->get ( ( unsigned int )name.getIndex ());
        }
        value = this->mObject->get ( name.getName ());
        
        if ( !value.isEmpty ()) {
            try {
                return value.convert < TYPE >();
            }
            catch ( ... ) {
            }
        }
        return fallback;
    }

    //----------------------------------------------------------------//
    void set ( SerializerPropertyName name, const Poco::Dynamic::Var& value ) {
        
        if ( name.isIndex ()) {
            assert ( !this->mObject );
            if ( !this->mArray ) {
                this->mArray = new Poco::JSON::Array ();
            }
            this->mArray->set (( unsigned int )name.getIndex (), value );
        }
        else {
            assert ( !this->mArray );
            if ( !this->mObject ) {
                this->mObject = new Poco::JSON::Object ();
            }
            this->mObject->set ( name.getName (), value );
        }
    }
};

} // namespace Volition
#endif
