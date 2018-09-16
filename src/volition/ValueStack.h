// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VALUESTACK_H
#define VOLITION_VALUESTACK_H

#include <volition/common.h>
#include <volition/AbstractValueStack.h>

namespace Volition {

template < typename TYPE > class ValueStack;

//================================================================//
// ValueStackTuple
//================================================================//
template < typename TYPE >
class ValueStackTuple {
private:

    friend class ValueStack < TYPE >;

    TYPE            mValue;
    size_t          mVersion;

    //----------------------------------------------------------------//
    ValueStackTuple ( const TYPE& value, size_t version ) :
        mValue ( value ),
        mVersion ( version ) {
    }
};

//================================================================//
// ValueStack
//================================================================//
template < typename TYPE >
class ValueStack :
    public AbstractValueStack {
protected:

    friend class VersionedStore;
    friend class VersionedStoreLayer;
    friend class VersionedStoreEpoch;

    vector < ValueStackTuple < TYPE >>  mValues;

    //----------------------------------------------------------------//
    const void* AbstractValueStack_getRaw ( size_t version ) const override {
        assert ( this->mValues.size () > 0 );
        
        // naive lookup
        // TODO: optimize
        
        for ( size_t i = this->mValues.size (); i > 0; --i ) {
            const ValueStackTuple < TYPE >& tuple = this->mValues [ i - 1 ];
            if ( tuple.mVersion <= version ) {
                return &tuple.mValue;
            }
        }
        return NULL;
    }
    
    //----------------------------------------------------------------//
    const void* AbstractValueStack_getRawForIndex ( size_t index ) const override {
    
        assert ( index < this->size ());
        const ValueStackTuple < TYPE >& tuple = this->mValues [ index ];
        return &tuple.mValue;
    }
    
    //----------------------------------------------------------------//
    size_t AbstractValueStack_getVersionForIndex ( size_t index ) const override {
    
        const ValueStackTuple < TYPE >& tuple = this->mValues [ index ];
        return tuple.mVersion;
    }
    
    //----------------------------------------------------------------//
    bool AbstractValueStack_isEmpty () const override {
        return ( this->mValues.size () == 0 );
    }
    
    //----------------------------------------------------------------//
    unique_ptr < AbstractValueStack > AbstractValueStack_makeEmptyCopy () const override {
        return make_unique < ValueStack < TYPE >>();
    }
    
    //----------------------------------------------------------------//
    void AbstractValueStack_pop () override {
        assert ( this->mValues.size () > 0 );
        this->mValues.pop_back ();
    }
        
    //----------------------------------------------------------------//
    void AbstractValueStack_pushBackRaw ( const void* value, size_t version ) override {
        assert ( value );
        this->mValues.push_back ( ValueStackTuple < TYPE >( *( const TYPE* )value, version ));
    }

    //----------------------------------------------------------------//
    size_t AbstractValueStack_size () const override {
    
        return this->mValues.size ();
    }

public:

    //----------------------------------------------------------------//
    ValueStack () {
        this->mTypeID = typeid ( TYPE ).hash_code ();
    }
};

} // namespace Volition
#endif
