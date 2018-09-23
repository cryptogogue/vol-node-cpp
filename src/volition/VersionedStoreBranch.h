// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTOREBRANCH_H
#define VOLITION_VERSIONEDSTOREBRANCH_H

#include <volition/common.h>
#include <volition/ValueStack.h>

namespace Volition {

class VersionedStoreBranchClient;

//================================================================//
// VersionedStoreBranch
//================================================================//
class VersionedStoreBranch :
    public enable_shared_from_this < VersionedStoreBranch > {
private:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreBranchClient;
    friend class VersionedStoreIterator;
    template < typename > friend class VersionedValueIterator;

    typedef set < string > BranchLayer;

    set < VersionedStoreBranchClient* >                       mClients;
    set < VersionedStoreBranch* >                           mChildren;
    map < size_t, BranchLayer >                             mBranchLayers;
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;

    shared_ptr < VersionedStoreBranch >                     mParent;
    size_t                                                  mBaseVersion;

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
    void                            affirmChild                 ( VersionedStoreBranch& child );
    void                            affirmClient                ( VersionedStoreBranchClient& client );
    size_t                          countDependencies           () const;
    void                            eraseChild                  ( VersionedStoreBranch& child );
    void                            eraseClient                 ( VersionedStoreBranchClient& client );
    size_t                          findImmutableTop            ( const VersionedStoreBranchClient* ignore = NULL ) const;
    const AbstractValueStack*       findValueStack              ( string key ) const;
    const void*                     getRaw                      ( size_t version, string key, size_t typeID ) const;
    size_t                          getTopVersion               () const;
    size_t                          getVersionDependency        () const;
    void                            optimize                    ();
    void                            popLayer                    ();
    void                            setParent                   ( shared_ptr < VersionedStoreBranch > parent );
    void                            setRaw                      ( size_t version, string key, const void* value );

public:

    //----------------------------------------------------------------//
                                    VersionedStoreBranch        ();
                                    VersionedStoreBranch        ( shared_ptr < VersionedStoreBranch > parent, size_t baseVersion );
                                    ~VersionedStoreBranch       ();
};

} // namespace Volition
#endif
