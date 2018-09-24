// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDVALUE_H
#define VOLITION_VERSIONEDVALUE_H

#include <volition/common.h>
#include <volition/VersionedStoreSnapshot.h>

namespace Volition {

//================================================================//
// VersionedValue
//================================================================//
/** \brief Lightweight handle intended to safely reference a value held
    in a versioned store. Useful for large values that would be cumbersome to copy.
 
    The versioned store implementation only returns copies of values. This is for
    safety: in the current implementation, "optimization" of versioned branches
    may be triggered by iterators and other operations on the store. This could
    invalidate direct pointers and references to versioned values.
 
    VersionedValue disables optimization for the branch it references, thus
    preventing invalidation.
*/
template < typename TYPE >
class VersionedValue :
    public VersionedStoreSnapshot {
protected:

    /// Direct pointer to the value being referenced.
    const TYPE*         mValue;

    //----------------------------------------------------------------//
    /** \brief Set the value (and the store). Increment the branch direct reference count.
    
        This function casts away the const status on the versioned store parameter. This
        is to support the full range of copy constructor and assignment operator options.
        While we'd normally avoid casting away const, in this case we need to because
        versioned branches maintain a list of their clients (as well as a direct reference
        count used to prevent concatenation).
     
        Even though the internal versioned branch is modified, conceptually the versioned
        store is not: the values it returns remain unchanged.
    
        \param  versionedStore  The versioned store containing the value.
        \param  value           The value. Should be from versionedStore.
    */
    void setValue ( const VersionedStoreSnapshot& versionedStore, const TYPE* value ) {
    
        if ( value ) {
        
            VersionedStoreSnapshot& mutableStore = const_cast < VersionedStoreSnapshot& >( versionedStore );
            mutableStore.mBranch->mDirectReferenceCount++; // increment this first
        
            this->takeSnapshot ( mutableStore );
            this->mValue = value;
        }
    }

public:

    //----------------------------------------------------------------//
    /** \brief Clear the value and decrement the branch direct reference count.
    */
    void clear () {
    
        if ( this->mBranch ) {
            this->mBranch->mDirectReferenceCount--;
        }
        this->mValue = NULL;
        this->VersionedStoreSnapshot::clear ();
    }

    //----------------------------------------------------------------//
    /** \brief True if value is non-NULL.
    */
    operator bool () const {
        return ( this->mValue != NULL );
    }

    //----------------------------------------------------------------//
    /** \brief Cast operator to return a pointer to the value.
    */
    operator const TYPE* () const {
        return this->mValue;
    }

    //----------------------------------------------------------------//
    /** \brief Arrow operator into value.
    */
    const TYPE* operator-> () {
        assert ( this->mValue );
        return this->mValue;
    }

    //----------------------------------------------------------------//
    /** \brief Dereference operator.
    */
    const TYPE& operator * () const {
        assert ( this->mValue );
        return *this->mValue;
    }

    //----------------------------------------------------------------//
    /** \brief Assignment operator.
    
        \param  other   VersionedValue to copy from.
    */
    VersionedValue < TYPE >& operator = ( const VersionedValue < TYPE >& other ) {
        this->copy ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    /** \brief Copy from another VersionedValue.
    
        \param  other   VersionedValue to copy from.
    */
    void copy ( const VersionedValue < TYPE >& other ) {
        this->clear ();
        this->setValue ( other, other.mValue );
    }

    //----------------------------------------------------------------//
    /** \brief Inititalize and set to value for key.
    
        \param  versionedStore      VersionedValue to get value from.
        \param  key                 The key.
    */
    void setValue ( const VersionedStoreSnapshot& versionedStore, string key ) {
        
        this->clear ();
        
        const TYPE* value = versionedStore.getValueOrNil < TYPE >( key, versionedStore.getVersion ());
        this->setValue ( versionedStore, value );
    }
    
    //----------------------------------------------------------------//
    VersionedValue () :
        mValue ( NULL ) {
    }
    
    //----------------------------------------------------------------//
    /** \brief Copy constructor.
    
        \param  other   VersionedValue to copy from.
    */
    VersionedValue ( const VersionedValue < TYPE >& other ) {
        this->copy ( other );
    }
    
    //----------------------------------------------------------------//
    /** \brief Inititalize and set to value for key.
    
        \param  versionedStore      VersionedValue to get value from.
        \param  key                 The key.
    */
    VersionedValue ( const VersionedStoreSnapshot& versionedStore, string key ) {
        this->setValue ( versionedStore, key );
    }

    //----------------------------------------------------------------//
    ~VersionedValue () {
        this->clear ();
    }
};

} // namespace Volition
#endif
