// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTORE_H
#define VOLITION_VERSIONEDSTORE_H

#include <volition/common.h>
#include <volition/VersionedStoreSnapshot.h>

// TODO: this is all placeholder stuff, to get the algorithm working. will need to
// optimize to reduce dynamic allocation. will also need to provide a NoSQL-backed
// implementation.

namespace Volition {

//================================================================//
// VersionedStore
//================================================================//
/** \brief VersionedStore is a key/value store that can be rewound and branched into multiple
    versions. This is the "backbone" of the Volition blockchain implementation.
 
    The VersionedStoreSnapshot class represents a cursor into the versioned key/value store. The database
    itself is held in a series of branches (VersionedBranch). Branches may have multiple
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
    from VersionedStoreSnapshot and thus give access to any value in the store.
 
    Iterators are faster moving backward through the version history. Due to the branching nature
    of the store, iterating forward may incur additional overhead when a fork in a branch is
    reached. To navigate the fork, the iterator must restart at the end of the branch and search
    bakward through branches until the correct side of the fork is found.
 
    A versioned store is interesting for blockchain implementation because it can represent
    both the blockchain itself (each version contains a block) and the overall "state" of the
    ledger. This state can be rewound and branched as new blocks are evaluated for addition
    to the chain.
 
    \todo The current implementation heavily uses STL maps. During the optimization step,
    branches are combined through concatenation. We'll eventually want an implementation that
    can do this operation quickly.
 
    \todo The current implementation exists only as a local in-memory data structure. We'll need
    a way to back it to an in-memory database server backed by storage media, such as Redis.
*/
class VersionedStore :
    public VersionedStoreSnapshot {
protected:

    //----------------------------------------------------------------//
    void            prepareForSetValue              ();
    
public:

    //----------------------------------------------------------------//
    void            popVersion                      ();
    void            pushVersion                     ();
    void            revert                          ( size_t version );
                    VersionedStore                  ();
                    VersionedStore                  ( VersionedStoreSnapshot& other );
                    ~VersionedStore                 ();
    
    //----------------------------------------------------------------//
    /** \brief  Implements assignment by calling takeSnapshot().
     
        \param  other   The version to snapshot.
    */
    VersionedStore& operator = ( VersionedStoreSnapshot& other ) {
        this->takeSnapshot ( other );
        return *this;
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
        assert ( this->mBranch );
        this->mBranch->affirmValueStack < TYPE >( key );
        this->mBranch->setRaw ( this->mVersion, key, typeid ( TYPE ).hash_code (), &value );
    }
};

} // namespace Volition
#endif
