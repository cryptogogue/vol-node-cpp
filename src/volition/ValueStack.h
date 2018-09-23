// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VALUESTACK_H
#define VOLITION_VALUESTACK_H

#include <volition/common.h>
#include <volition/AbstractValueStack.h>

namespace Volition {

template < typename TYPE > class VersionedValueIterator;

//================================================================//
// ValueStack
//================================================================//
template < typename TYPE >
class ValueStack :
    public AbstractValueStack {
protected:

    friend class VersionedStoreBranchClient;
    friend class VersionedStoreLayer;
    friend class VersionedStoreBranch;
    friend class VersionedValueIterator < TYPE >;

    map < size_t, TYPE >    mValuesByVersion;

    //----------------------------------------------------------------//
    void AbstractValueStack_copyFrom ( const AbstractValueStack& from ) override {
    
        const ValueStack < TYPE >* fromPtr = dynamic_cast < const ValueStack < TYPE >* >( &from );
        assert ( fromPtr );
        this->mValuesByVersion.insert ( fromPtr->mValuesByVersion.begin (),  fromPtr->mValuesByVersion.end ());
    }

    //----------------------------------------------------------------//
    void AbstractValueStack_erase ( size_t version ) override {
        this->mValuesByVersion.erase ( version );
    }

    //----------------------------------------------------------------//
    const void* AbstractValueStack_getRaw ( size_t version ) const override {
        
        if ( this->mValuesByVersion.size ()) {
        
            typename map < size_t, TYPE >::const_iterator valueIt = this->mValuesByVersion.lower_bound ( version );
            
            if ( valueIt == this->mValuesByVersion.cend ()) {
                return &this->mValuesByVersion.rbegin ()->second;
            }
            else {
            
                if ( valueIt->first > version ) {
                    valueIt--;
                }
                return &valueIt->second;
            }
        }
        return NULL;
    }
    
    //----------------------------------------------------------------//
    unique_ptr < AbstractValueStack > AbstractValueStack_makeEmptyCopy () const override {
        return make_unique < ValueStack < TYPE >>();
    }
    
    //----------------------------------------------------------------//
    void AbstractValueStack_setRaw ( size_t version, const void* value ) override {
        assert ( value );
        this->mValuesByVersion [ version ] = *( TYPE* )value;
    }

    //----------------------------------------------------------------//
    size_t AbstractValueStack_size () const override {
    
        return this->mValuesByVersion.size ();
    }

public:

    //----------------------------------------------------------------//
    ValueStack () {
        this->mTypeID = typeid ( TYPE ).hash_code ();
    }
};

} // namespace Volition
#endif
