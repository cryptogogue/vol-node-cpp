// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_ABSTRACTSERIALIZER_H
#define VOLITION_SERIALIZATION_ABSTRACTSERIALIZER_H

#include <volition/common.h>

namespace Volition {

class AbstractSerializable;

//================================================================//
// SerializerPropertyName
//================================================================//
class SerializerPropertyName {
protected:

    bool        mIsIndex;
    size_t      mIndex;
    string      mName;

public:

    //----------------------------------------------------------------//
    size_t getIndex () {
        assert ( this->mIsIndex );
        return this->mIndex;
    }

    //----------------------------------------------------------------//
    string getName () {
        assert ( !this->mIsIndex );
        return this->mName;
    }

    //----------------------------------------------------------------//
    SerializerPropertyName ( size_t index ) :
        mIsIndex ( true ),
        mIndex ( index ) {
    }
    
    //----------------------------------------------------------------//
    SerializerPropertyName ( const char* name ) :
        mIsIndex ( false ),
        mName ( name ) {
    }
    
    //----------------------------------------------------------------//
    SerializerPropertyName ( string name ) :
        mIsIndex ( false ),
        mName ( name ) {
    }
};

//================================================================//
// AbstractSerializer
//================================================================//
class AbstractSerializer {
public:

    enum Mode {
        SERIALIZE_DIGEST,
        SERIALIZE_IN,
        SERIALIZE_OUT,
    };

protected:

    //----------------------------------------------------------------//
    virtual Mode        AbstractSerializer_getMode          () = 0;
    virtual size_t      AbstractSerializer_getSize          () = 0;
    virtual bool        AbstractSerializer_has              ( SerializerPropertyName name ) = 0;
    virtual void        AbstractSerializer_serialize        ( SerializerPropertyName name, u64& value ) = 0;
    virtual void        AbstractSerializer_serialize        ( SerializerPropertyName name, string& value ) = 0;
    virtual void        AbstractSerializer_serialize        ( SerializerPropertyName name, AbstractSerializable& value ) = 0;
    virtual void        AbstractSerializer_serialize        ( SerializerPropertyName name, AbstractSerializableArray& value ) = 0;
    virtual void        AbstractSerializer_serialize        ( SerializerPropertyName name, AbstractSerializablePointer& value ) = 0;
    virtual void        AbstractSerializer_serialize        ( SerializerPropertyName name, unique_ptr < Poco::Crypto::ECKey >& key ) = 0;
    virtual void        AbstractSerializer_serialize        ( SerializerPropertyName name, Poco::DigestEngine::Digest& value ) = 0;

public:

    //----------------------------------------------------------------//
    Mode getMode () {
        return this->AbstractSerializer_getMode ();
    }
    
    //----------------------------------------------------------------//
    size_t getSize () {
        return this->AbstractSerializer_getSize ();
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void serialize ( SerializerPropertyName name, TYPE& value ) {
        this->AbstractSerializer_serialize ( name, value );
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    TYPE serializeIn ( SerializerPropertyName name, const TYPE& fallback ) {
        assert ( this->getMode () == SERIALIZE_IN );
        TYPE value = fallback;
        this->AbstractSerializer_serialize ( name, value );
        return value;
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void serializeOut ( SerializerPropertyName name, const TYPE& value ) {
        assert (( this->getMode () == SERIALIZE_DIGEST ) || ( this->getMode () == SERIALIZE_OUT ));
        TYPE temp = value;
        this->AbstractSerializer_serialize ( name, temp );
    }
};

} // namespace Volition
#endif
