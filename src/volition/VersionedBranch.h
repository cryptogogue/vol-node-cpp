// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDBRANCH_H
#define VOLITION_VERSIONEDBRANCH_H

#include <volition/common.h>
#include <volition/AbstractVersionedBranchClient.h>
#include <volition/ValueStack.h>

namespace Volition {

class AbstractVersionedBranchClient;
class VersionedStoreSnapshot;

//================================================================//
// VersionedBranch
//================================================================//
/** \brief VersionedBranch is an internal data structure used to store
    a contiguous block of versioned values.
 
    Each branch is a sparse record of changes to the database over a
    span of versions. It contains a map of ValueStack instances referenced
    by key. In addition, a sparse stack of version layers (implemented as a map of
    string sets) is used to quickly identify the keys of values modified in that layer.
 
    The branch tracks a base version. Its "top" version is the highest version
    index in the layer stack. When a layer is removed, the keys of the values that were
    set in that layer are retrieved from the layer stack and used to erase the
    corresponding values from each value stack.
 
    Any branch may have a parent branch. When searching for values, the tree will
    be searched recurively until a value is found.
*/
class VersionedBranch :
    public enable_shared_from_this < VersionedBranch >,
    public AbstractVersionedBranchClient {
private:

    friend class AbstractVersionedBranchClient;
    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreSnapshot;
    friend class VersionedStoreIterator;
    template < typename > friend class VersionedValue;
    template < typename > friend class VersionedValueIterator;

    typedef set < string > Layer;

    /// Set containing active clients. This is needed to calculate dependencies and to know when to optimize.
    set < AbstractVersionedBranchClient* >                  mClients;
    
    /// Sparse array mapping versions onto layers. Each layer holds the set of keys corresponding to values that were set or modified in the version.
    map < size_t, Layer >                                   mLayers;
    
    /// Map of value stacks, indexed by key.
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;

    /// The number of clients holding direct references to branch internals. A nonzero direct reference count will prevent optimization of the branch.
    size_t                                                  mDirectReferenceCount;

    //----------------------------------------------------------------//
    /** \brief Creates a typed instance of ValueStack for the given key if none exists.
    */
    template < typename TYPE >
    void affirmValueStack ( string key ) {
        unique_ptr < AbstractValueStack >& versionedValue = this->mValueStacksByKey [ key ];
        if ( !versionedValue ) {
            versionedValue = make_unique < ValueStack < TYPE >>();
        }
        assert ( versionedValue->mTypeID == typeid ( TYPE ).hash_code ());
    }

    //----------------------------------------------------------------//
    size_t                          countDependencies           () const;
    void                            eraseClient                 ( AbstractVersionedBranchClient& client );
    size_t                          findImmutableTop            ( const AbstractVersionedBranchClient* ignore = NULL ) const;
    const AbstractValueStack*       findValueStack              ( string key ) const;
    shared_ptr < VersionedBranch >  fork                        ( size_t baseVersion );
    const void*                     getRaw                      ( size_t version, string key, size_t typeID ) const;
    size_t                          getTopVersion               () const;
    void                            insertClient                ( AbstractVersionedBranchClient& client );
    void                            optimize                    ();
    void                            setRaw                      ( size_t version, string key, size_t typeID, const void* value );
    void                            truncate                    ( size_t topVersion );

    //----------------------------------------------------------------//
    bool            AbstractVersionedStoreClient_canJoin                    () const override;
    size_t          AbstractVersionedStoreClient_getJoinScore               () const override;
    size_t          AbstractVersionedStoreClient_getVersionDependency       () const override;
    void            AbstractVersionedStoreClient_joinBranch                 ( VersionedBranch& branch ) override;
    bool            AbstractVersionedStoreClient_preventJoin                () const override;

public:

    //----------------------------------------------------------------//
                    VersionedBranch         ();
                    ~VersionedBranch        ();
};

} // namespace Volition
#endif
