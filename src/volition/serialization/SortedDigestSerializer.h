// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SORTEDDIGESTSERIALIZER_H
#define VOLITION_SERIALIZATION_SORTEDDIGESTSERIALIZER_H

#include <volition/serialization/AbstractSerializer.h>

namespace Volition {

//================================================================//
// AbstractSortedDigestSerializerValue
//================================================================//
class AbstractSortedDigestSerializerValue {
protected:

    //----------------------------------------------------------------//
    virtual void    AbstractSortedDigestSerializerValue_digest  ( Poco::DigestOutputStream& stream ) const = 0;

public:

    //----------------------------------------------------------------//
    AbstractSortedDigestSerializerValue () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractSortedDigestSerializerValue () {
    }
    
    //----------------------------------------------------------------//
    void digest ( Poco::DigestOutputStream& stream ) const {
        this->AbstractSortedDigestSerializerValue_digest ( stream );
    }
};

//================================================================//
// SortedDigestSerializerValue
//================================================================//
template < typename TYPE >
class SortedDigestSerializerValue :
    public AbstractSortedDigestSerializerValue {
private:

    TYPE mValue;

    //----------------------------------------------------------------//
    void AbstractSortedDigestSerializerValue_digest ( Poco::DigestOutputStream& stream ) const override {
        stream << this->mValue;
    }

public:
    
    //----------------------------------------------------------------//
    SortedDigestSerializerValue ( TYPE value ) :
        mValue ( value ) {
    }
};

//================================================================//
// AbstractSortedDigestSerializerContainer
//================================================================//
class AbstractSortedDigestSerializerContainer :
    public AbstractSortedDigestSerializerValue {
protected:

    //----------------------------------------------------------------//
    virtual void    AbstractSortedDigestSerializerContainer_setValue   ( SerializerPropertyName name, unique_ptr < AbstractSortedDigestSerializerValue > value ) = 0;

public:

    //----------------------------------------------------------------//
    void setValue ( SerializerPropertyName name, unique_ptr < AbstractSortedDigestSerializerValue > value ) {
        this->AbstractSortedDigestSerializerContainer_setValue ( name, move ( value ));
    }
};

//================================================================//
// SortedDigestSerializerValueArray
//================================================================//
class SortedDigestSerializerValueArray :
    public AbstractSortedDigestSerializerContainer {
private:

    vector < unique_ptr < AbstractSortedDigestSerializerValue >> mValues;

    //----------------------------------------------------------------//
    void AbstractSortedDigestSerializerValue_digest ( Poco::DigestOutputStream& stream ) const override {
        
        size_t size = this->mValues.size ();
        for ( size_t i = 0; i < size; ++i ) {
            if ( this->mValues [ i ]) {
                this->mValues [ i ]->digest ( stream );
            }
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSortedDigestSerializerContainer_setValue ( SerializerPropertyName name, unique_ptr < AbstractSortedDigestSerializerValue > value ) override {
        size_t index = name.getIndex ();
        size_t minSize = index + 1;
        if ( this->mValues.size () < minSize ) {
            this->mValues.resize ( minSize );
        }
        this->mValues [ name.getIndex ()] = move ( value );
    }
};

//================================================================//
// SortedDigestSerializerObject
//================================================================//
class SortedDigestSerializerObject :
    public AbstractSortedDigestSerializerContainer {
private:

    map < string, unique_ptr < AbstractSortedDigestSerializerValue >> mValues;

    //----------------------------------------------------------------//
    void AbstractSortedDigestSerializerValue_digest ( Poco::DigestOutputStream& stream ) const override {
        
        map < string, unique_ptr < AbstractSortedDigestSerializerValue >>::const_iterator valueIt = this->mValues.cbegin ();
        for ( ; valueIt != this->mValues.cend (); ++valueIt ) {
            valueIt->second->digest ( stream );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSortedDigestSerializerContainer_setValue ( SerializerPropertyName name, unique_ptr < AbstractSortedDigestSerializerValue > value ) override {
        this->mValues [ name.getName ()] = move ( value );
    }
};

//================================================================//
// SortedDigestSerializer
//================================================================//
class SortedDigestSerializer :
    public AbstractSerializer {
private:

    unique_ptr < AbstractSortedDigestSerializerContainer > mContainer;

protected:

    //----------------------------------------------------------------//
    Mode AbstractSerializer_getMode () override {
        return SERIALIZE_DIGEST;
    }
    
    //----------------------------------------------------------------//
    size_t AbstractSerializer_getSize () override {
        return 0;
    }

    //----------------------------------------------------------------//
    bool AbstractSerializer_has ( SerializerPropertyName name ) override {
        return false;
    }

    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, u64& value ) override {
        assert ( this->mContainer );
        this->mContainer->setValue ( name, make_unique < SortedDigestSerializerValue < u64 >>( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, string& value ) override {
        assert ( this->mContainer );
        this->mContainer->setValue ( name, make_unique < SortedDigestSerializerValue < string >>( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializable& value ) override {
    
        SortedDigestSerializer serializer ( make_unique < SortedDigestSerializerObject >());
        value.serialize ( serializer );
        this->mContainer->setValue ( name, move ( serializer.mContainer ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializableArray& value ) override {
    
        SortedDigestSerializer serializer ( make_unique < SortedDigestSerializerValueArray >());
        value.serialize ( serializer );
        this->mContainer->setValue ( name, move ( serializer.mContainer ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractSerializablePointer& value ) override {
    
        AbstractSerializable* serializable = value.AbstractSerializablePointer_get ();
        if ( serializable ) {
            this->AbstractSerializer_serialize ( name, *serializable );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializer_serialize ( SerializerPropertyName name, AbstractStringifiable& value ) override {
        assert ( this->mContainer );
        this->mContainer->setValue ( name, make_unique < SortedDigestSerializerValue < string >>( value.toString ()));
    }

public:

    //----------------------------------------------------------------//
    SortedDigestSerializer ( unique_ptr < AbstractSortedDigestSerializerContainer > container ) :
        mContainer ( move ( container )) {
    }

    //----------------------------------------------------------------//
    static void hash ( AbstractSerializable& serializable, Poco::DigestOutputStream& digestStream ) {

        SortedDigestSerializer serializer ( make_unique < SortedDigestSerializerObject >());
        serializable.serialize ( serializer );
        serializer.mContainer->digest ( digestStream );
    }
};

} // namespace Volition
#endif
