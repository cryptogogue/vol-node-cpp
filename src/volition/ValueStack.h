// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VALUESTACK_H
#define VOLITION_VALUESTACK_H

#include <volition/common.h>
#include <volition/AbstractValueStack.h>

namespace Volition {

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

    vector < TYPE >     mValues;

    //----------------------------------------------------------------//
    const void* AbstractValueStack_getRaw () const override {
        assert ( this->mValues.size () > 0 );
        return &this->mValues.back ();
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
    void AbstractValueStack_pushBackRaw ( const void* value ) override {
        assert ( value );
        this->mValues.push_back ( *( const TYPE* )value );
    }
    
    //----------------------------------------------------------------//
    void AbstractValueStack_pushFrontRaw ( const void* value ) override {
        assert ( value );
        this->mValues.insert ( this->mValues.begin (), *( const TYPE* )value );
    }

public:

    //----------------------------------------------------------------//
    ValueStack () {
        this->mTypeID = typeid ( TYPE ).hash_code ();
    }
};

} // namespace Volition
#endif
