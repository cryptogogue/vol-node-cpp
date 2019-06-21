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

    SortedDigestSerializer*     mParent;
    SerializerPropertyName      mName;

protected:

    //----------------------------------------------------------------//
    void AbstractSerializerTo_affirmArray () override {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_affirmObject () override {
    }

    //----------------------------------------------------------------//
    SerializerPropertyName AbstractSerializerTo_getName () const override {
        return this->mName;
    }
    
    //----------------------------------------------------------------//
    AbstractSerializerTo* AbstractSerializerTo_getParent () override {
        return this->mParent;
    }

    //----------------------------------------------------------------//
    bool AbstractSerializerTo_isDigest () const override {
        return true;
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const bool& value ) override {
    
        this->setValue ( name, make_unique < SortedDigestSerializerValue < bool >>( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const double& value ) override {
    
        this->setValue ( name, make_unique < SortedDigestSerializerValue < double >>( value ));
    }

    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const u64& value ) override {
    
        this->setValue ( name, make_unique < SortedDigestSerializerValue < u64 >>( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const string& value ) override {
    
        this->setValue ( name, make_unique < SortedDigestSerializerValue < string >>( value ));
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const AbstractSerializable& value ) override {
    
        SortedDigestSerializer serializer;
        serializer.mParent = this;
        serializer.mName = name;
        value.serialize ( serializer );
        if ( serializer.mContainer ) {
            this->setValue ( name, move ( serializer.mContainer ));
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializerTo_serialize ( SerializerPropertyName name, const SerializationFunc& serializeFunc ) override {
    
        SortedDigestSerializer serializer;
        serializer.mParent = this;
        serializer.mName = name;
        serializeFunc ( serializer );
        if ( serializer.mContainer ) {
            this->setValue ( name, move ( serializer.mContainer ));
        }
    }

    //----------------------------------------------------------------//
    void AbstractSerializerFrom_stringToTree ( SerializerPropertyName name, string value ) override {
        assert ( false );
    }

    //----------------------------------------------------------------//
    void setValue ( SerializerPropertyName name, unique_ptr < AbstractSortedDigestSerializerValue > value ) {
    
        if ( !this->mContainer ) {
            if ( name.isIndex ()) {
                this->mContainer = make_unique < SortedDigestSerializerValueArray >();
            }
            else {
                this->mContainer = make_unique < SortedDigestSerializerObject >();
            }
        }
        this->mContainer->setValue ( name, move ( value ));
    }

public:

    //----------------------------------------------------------------//
    static void hash ( const AbstractSerializable& serializable, Poco::DigestOutputStream& digestStream ) {

        SortedDigestSerializer serializer;
        serializable.serialize ( serializer );
        serializer.mContainer->digest ( digestStream );
    }
};

} // namespace Volition
#endif
