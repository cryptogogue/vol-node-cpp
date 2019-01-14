// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_JSONSERIALIZER_H
#define VOLITION_SERIALIZATION_JSONSERIALIZER_H

namespace Volition {

//================================================================//
// JSONSerializer
//================================================================//
template < typename ARRAY_TYPE, typename OBJECT_TYPE >
class JSONSerializer {
protected:
    
    ARRAY_TYPE*     mArray;
    OBJECT_TYPE*    mObject;

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
        mArray ( NULL ) {
    }

    //----------------------------------------------------------------//
    const Poco::Dynamic::Var get ( SerializerPropertyName name ) const {
    
        if ( this->mArray ) {
            return this->mArray->get (( unsigned int )name.getIndex ());
        }
        return this->mObject->get ( name.getName ());
    }

//    //----------------------------------------------------------------//
//    const Poco::JSON::Array::Ptr getArray ( SerializerPropertyName name ) const {
//
//        if ( this->mArray ) {
//            return this->mArray->getArray (( unsigned int )name.getIndex ());
//        }
//        return this->mObject->getArray ( name.getName ());
//    }
//
//    //----------------------------------------------------------------//
//    const Poco::JSON::Object::Ptr getObject ( SerializerPropertyName name ) const {
//
//        if ( this->mArray ) {
//            return this->mArray->getObject (( unsigned int )name.getIndex ());
//        }
//        return this->mObject->getObject ( name.getName ());
//    }

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
    
        if ( this->mArray ) {
            return this->mArray->optElement (( unsigned int )name.getIndex (), fallback );
        }
        return this->mObject->optValue ( name.getName (), fallback );
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
