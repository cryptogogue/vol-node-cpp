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
/** \brief Templated implementation of AbstractValueStack.
*/
template < typename TYPE >
class ValueStack :
    public AbstractValueStack {
protected:

    friend class VersionedStoreSnapshot;
    friend class VersionedBranch;
    friend class VersionedStoreLayer;
    friend class VersionedValueIterator < TYPE >;

    map < size_t, TYPE >    mValuesByVersion;

    //----------------------------------------------------------------//
    void AbstractValueStack_copyFrom ( const AbstractValueStack& from ) override {
    
        const ValueStack < TYPE >* valueStack = dynamic_cast < const ValueStack < TYPE >* >( &from );
        assert ( valueStack );
        this->mValuesByVersion.insert ( valueStack->mValuesByVersion.begin (),  valueStack->mValuesByVersion.end ());
    }

    //----------------------------------------------------------------//
    void AbstractValueStack_copyValueFrom ( const AbstractValueStack& from, size_t version ) override {
    
        const ValueStack < TYPE >* valueStack = dynamic_cast < const ValueStack < TYPE >* >( &from );
        assert ( valueStack );
        
        const TYPE* value = valueStack->getValueOrNil ( version );
        if ( value ) {
            this->setValue ( version, *value );
        }
    }

    //----------------------------------------------------------------//
    void AbstractValueStack_erase ( size_t version ) override {
        this->mValuesByVersion.erase ( version );
    }
    
    //----------------------------------------------------------------//
    unique_ptr < AbstractValueStack > AbstractValueStack_makeEmptyCopy () const override {
        return make_unique < ValueStack < TYPE >>();
    }

    //----------------------------------------------------------------//
    size_t AbstractValueStack_size () const override {
    
        return this->mValuesByVersion.size ();
    }
    
    //================================================================//
    // overrides
    //================================================================//
    
    //----------------------------------------------------------------//
    const TYPE* getValueOrNil ( size_t version ) const {
        
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
    void setValue ( size_t version, const TYPE& value ) {
        this->mValuesByVersion [ version ] = value;
    }

public:

    //----------------------------------------------------------------//
    ValueStack () {
    }
};

} // namespace Volition
#endif
