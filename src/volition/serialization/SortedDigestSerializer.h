// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SORTEDDIGESTSERIALIZER_H
#define VOLITION_SERIALIZATION_SORTEDDIGESTSERIALIZER_H

#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

// TODO: we should be able to speed this up with a mem pool

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
    public AbstractSerializerTo {
private:

    unique_ptr < AbstractSortedDigestSerializerContainer > mContainer;

protected:

    //----------------------------------------------------------------//
    bool AbstractSerializerTo_isDigest () const override {
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const u64& value ) override {
        assert ( this->mContainer );
        this->mContainer->setValue ( name, make_unique < SortedDigestSerializerValue < u64 >>( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const string& value ) override {
        assert ( this->mContainer );
        this->mContainer->setValue ( name, make_unique < SortedDigestSerializerValue < string >>( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializable& value ) override {
    
        SortedDigestSerializer serializer ( make_unique < SortedDigestSerializerObject >());
        value.serializeTo ( serializer );
        this->mContainer->setValue ( name, move ( serializer.mContainer ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializableArray& value ) override {
    
        SortedDigestSerializer serializer ( make_unique < SortedDigestSerializerValueArray >());
        value.serializeTo ( serializer );
        this->mContainer->setValue ( name, move ( serializer.mContainer ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializablePointer& value ) override {
    
        const AbstractSerializable* serializable = value.AbstractSerializablePointer_get ();
        if ( serializable ) {
            this->AbstractSerializerTo_serialize ( name, *serializable );
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractStringifiable& value ) override {
        assert ( this->mContainer );
        this->mContainer->setValue ( name, make_unique < SortedDigestSerializerValue < string >>( value.toString ()));
    }

public:

    //----------------------------------------------------------------//
    SortedDigestSerializer ( unique_ptr < AbstractSortedDigestSerializerContainer > container ) :
        mContainer ( move ( container )) {
    }

    //----------------------------------------------------------------//
    static void hash ( const AbstractSerializable& serializable, Poco::DigestOutputStream& digestStream ) {

        SortedDigestSerializer serializer ( make_unique < SortedDigestSerializerObject >());
        serializable.serializeTo ( serializer );
        serializer.mContainer->digest ( digestStream );
    }
};

} // namespace Volition
#endif
