// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTOREBRANCH_H
#define VOLITION_VERSIONEDSTOREBRANCH_H

#include <volition/common.h>
#include <volition/AbstractVersionedStoreClient.h>
#include <volition/ValueStack.h>

namespace Volition {

class AbstractVersionedStoreClient;
class VersionedStoreSnapshot;

//================================================================//
// VersionedStoreBranch
//================================================================//
/** \brief VersionedStoreBranch is an internal data structure used to store
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
class VersionedStoreBranch :
    public enable_shared_from_this < VersionedStoreBranch >,
    public AbstractVersionedStoreClient {
private:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreSnapshot;
    friend class VersionedStoreIterator;
    template < typename > friend class VersionedValue;
    template < typename > friend class VersionedValueIterator;

    typedef set < string > Layer;

    set < AbstractVersionedStoreClient* >                   mClients;
    map < size_t, Layer >                                   mLayers;
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;

    size_t                                                  mBaseVersion;

    size_t                                                  mDirectReferenceCount;

    //----------------------------------------------------------------//
    template < typename TYPE >
    void affirmValueStack ( string key ) {
        unique_ptr < AbstractValueStack >& versionedValue = this->mValueStacksByKey [ key ];
        if ( !versionedValue ) {
            versionedValue = make_unique < ValueStack < TYPE >>();
        }
        assert ( versionedValue->mTypeID == typeid ( TYPE ).hash_code ());
    }

    //----------------------------------------------------------------//
    void                            affirmClient                ( AbstractVersionedStoreClient& client );
    size_t                          countDependencies           () const;
    void                            eraseClient                 ( AbstractVersionedStoreClient& client );
    size_t                          findImmutableTop            ( const AbstractVersionedStoreClient* ignore = NULL ) const;
    const AbstractValueStack*       findValueStack              ( string key ) const;
    const void*                     getRaw                      ( size_t version, string key, size_t typeID ) const;
    size_t                          getTopVersion               () const;
    void                            optimize                    ();
    void                            setParent                   ( shared_ptr < VersionedStoreBranch > parent );
    void                            setRaw                      ( size_t version, string key, const void* value );
    void                            truncate                    ( size_t topVersion );

    //----------------------------------------------------------------//
    bool            AbstractVersionedStoreClient_canJoin                    () const override;
    size_t          AbstractVersionedStoreClient_getJoinScore               () const override;
    size_t          AbstractVersionedStoreClient_getVersionDependency       () const override;
    void            AbstractVersionedStoreClient_joinBranch                 ( VersionedStoreBranch& branch ) override;
    bool            AbstractVersionedStoreClient_preventJoin                () const override;

public:

    //----------------------------------------------------------------//
                    VersionedStoreBranch        ();
                    VersionedStoreBranch        ( shared_ptr < VersionedStoreBranch > parent, size_t baseVersion );
                    ~VersionedStoreBranch       ();
};

} // namespace Volition
#endif
