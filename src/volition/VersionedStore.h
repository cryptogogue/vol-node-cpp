// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTORE_H
#define VOLITION_VERSIONEDSTORE_H

#include <volition/common.h>
#include <volition/VersionedStoreBranch.h>

// TODO: this is all placeholder stuff, to get the algorithm working. will need to
// optimize to reduce dynamic allocation. will also need to provide a NoSQL-backed
// implementation.

namespace Volition {

//================================================================//
// VersionedStore
//================================================================//
/** \brief  VersionedStore is a key/value store that can be rewound and branched into multiple
            versions. This is the "backbone" of the Volition blockchain implementation.
 
    The VersionedStore class represents a cursor into the versioned key/value store. The database
    itself is held in a series of branches (VersionedStoreBranch). Branches may have multiple
    dependencies in the form of cursors and child branches.
 
    New branches are created as needed. For example, when a value is set for a key in a branch with
    multiple dependencies, a new branch will be created to hold the value so as not to overwrite the
    original.
 
    Garbage collection is also performed on branches. Branches are automatically "optimized" when
    cursors or versions are removed. Optimization includes appending child branches, when possible.
 
    For each version, values are stored sparsely. Values are held in stacks (ValusStack,
    AbstractValueStack). Each branch only contains stacks for values set in that branch. When
    a value is read, branches are searched recursively until the "top" value is found.
 
    Two iterator implementations are provided: VersionedStoreIterator and VersionedValueIterator.
    VersionedStoreIterator iterates through versions sequentially. VersionedValueIterator only
    visits versions where the value being iterated was set of changed. Both iterators inherit
    from VersionedStore and thus give access to any value in the store.
 
    Iterators are faster moving backward through the version history. Due to the branching nature
    of the store, iterating forward may incur additional overhead when a fork in a branch is
    reached. To navigate the fork, the iterator must restart at the end of the branch and search
    bakward through branches until the correct side of the fork is found.
*/
class VersionedStore {
protected:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreBranch;
    friend class VersionedStoreIterator;
    template < typename > friend class VersionedValueIterator;

    /// Pointer to the branch containg the current version.
    shared_ptr < VersionedStoreBranch >     mBranch;
    
    /// Current version of the store. Values set will be set at this version.
    size_t                                  mVersion;

    #ifdef _DEBUG
        /// Available in debug builds to add an easily readable name to cursors.
        string                              mDebugName;
    #endif

    //----------------------------------------------------------------//
    void            affirmBranch            ();
    const void*     getRaw                  ( string key, size_t version, size_t typeID ) const;
    size_t          getVersionDependency    () const;
    void            prepareForSetValue      ();
    void            setBranch               ( shared_ptr < VersionedStoreBranch > epoch, size_t version );
    void            setRaw                  ( string key, const void* value );
    
public:

    //----------------------------------------------------------------//
    void            clear                   ();
    size_t          getVersion              () const;
    void            popVersion              ();
    void            pushVersion             ();
    void            revert                  ( size_t version );
    void            setDebugName            ( string debugName );
    void            takeSnapshot            ( VersionedStore& other );
                    VersionedStore          ();
                    VersionedStore          ( VersionedStore& other );
                    ~VersionedStore         ();
    
    //----------------------------------------------------------------//
    /** \brief  Implements assignment by calling takeSnapshot().
     
        \param  other   The version to snapshot.
    */
    VersionedStore& operator = ( VersionedStore& other ) {
        this->takeSnapshot ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    /** \brief  Return a copy of the value for a key. Throws an exception if the
                value cannot be found.
     
        \param  key     The key.
        \return         A copy of the value.
    */
    template < typename TYPE >
    const TYPE& getValue ( string key ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key );
        assert ( value );
        return *value;
    }

    //----------------------------------------------------------------//
    /** \brief  Return a copy of the value for a key at a given version.
                Throws an exception if the value cannot be found.
     
                Returns the value for the most recent version equal to or less than
                the given version.
     
        \param  key         The key.
        \param  version     The version.
        \return             A copy of the value.
    */
    template < typename TYPE >
    const TYPE& getValue ( string key, size_t version ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key, version );
        assert ( value );
        return *value;
    }

    //----------------------------------------------------------------//
    /** \brief  Return a pointer to the value for a key or NULL if the
                value cannot be found.
     
        \param  key     The key.
        \return         A pointer to the value or NULL.
    */
    template < typename TYPE >
    const TYPE* getValueOrNil ( string key ) const {
        return ( TYPE* )this->getRaw ( key, this->mVersion, typeid ( TYPE ).hash_code ());
    }

    //----------------------------------------------------------------//
    /** \brief  Return a pointer to the value for a key at a given version
                or NULL if the value cannot be found.
     
                Returns the value for the most recent version equal to or less than
                the given version.
     
        \param  key         The key.
        \param  version     The version.
        \return             A pointer to the value or NULL.
    */
    template < typename TYPE >
    const TYPE* getValueOrNil ( string key, size_t version ) const {
        return ( TYPE* )this->getRaw ( key, version, typeid ( TYPE ).hash_code ());
    }

    //----------------------------------------------------------------//
    /** \brief  Check to see if the value can be found.
     
        \param  key     The key.
        \return         True if the value exists. False if it cant.
    */
    template < typename TYPE >
    bool hasTypedValue ( string key ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key );
        return ( value != NULL );
    }

    //----------------------------------------------------------------//
    /** \brief  Sets or overwrites the value for the corresponding key at the
                current version.
     
        \param  key         The key.
        \param  value       The value.
    */
    template < typename TYPE >
    void setValue ( string key, const TYPE& value ) {
        this->prepareForSetValue ();
        this->mBranch->affirmValueStack < TYPE >( key );
        this->setRaw ( key, &value );
    }
};

} // namespace Volition
#endif
