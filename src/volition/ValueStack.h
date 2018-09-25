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

    /// Sparse stack of values indexed by version.
    map < size_t, TYPE >    mValuesByVersion;

    //----------------------------------------------------------------//
    /** \brief Implementation of virtual method.
    
        \param      from        The value stack to copy from.
        \param      version     The version of the value to copy.
    */
    void AbstractValueStack_copyValueFrom ( const AbstractValueStack& from, size_t version ) override {
    
        const ValueStack < TYPE >* valueStack = dynamic_cast < const ValueStack < TYPE >* >( &from );
        assert ( valueStack );
        
        const TYPE* value = valueStack->getValueOrNil ( version );
        if ( value ) {
            this->setValue ( version, *value );
        }
    }

    //----------------------------------------------------------------//
    /** \brief Implementation of virtual method.
    
        \param      version     The version of the value to erase.
    */
    void AbstractValueStack_erase ( size_t version ) override {
        this->mValuesByVersion.erase ( version );
    }
    
    //----------------------------------------------------------------//
    /** \brief Implementation of virtual method.
    
        \param      from        The stack to copy from.
    */
    void AbstractValueStack_join ( AbstractValueStack& to ) const override {
    
        ValueStack < TYPE >* valueStack = dynamic_cast < ValueStack < TYPE >* >( &to );
        assert ( valueStack );
        if ( this->mValuesByVersion.size () > 0 ) {
            assert (( valueStack->mValuesByVersion.size () == 0 ) || ( valueStack->mValuesByVersion.crbegin ()->first < this->mValuesByVersion.cbegin ()->first ));
            valueStack->mValuesByVersion.insert ( this->mValuesByVersion.begin (),  this->mValuesByVersion.end ());
        }
    }
    
    //----------------------------------------------------------------//
    /** \brief Implementation of virtual method.
    
        \return     A new AbstractValueStack with the same type implementation as the source.
    */
    unique_ptr < AbstractValueStack > AbstractValueStack_makeEmptyCopy () const override {
        return make_unique < ValueStack < TYPE >>();
    }

    //----------------------------------------------------------------//
    /** \brief Implementation of virtual method.
    
        \return     The size of the stack.
    */
    size_t AbstractValueStack_size () const override {
    
        return this->mValuesByVersion.size ();
    }
    
    //================================================================//
    // overrides
    //================================================================//
    
    //----------------------------------------------------------------//
    /** \brief  Returns a pointer to the value for a key at a given version
                or NULL if the value cannot be found.
     
                Returns the value for the most recent version equal to or less than
                the given version.
     
        \param  key         The key.
        \param  version     The version.
        \return             A pointer to the value or NULL.
    */
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
    /** \brief  Sets or overwrites the value for the corresponding key at the
                current version.
     
        \param  key         The key.
        \param  value       The value.
    */
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
