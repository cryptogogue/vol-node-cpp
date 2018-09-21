// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTOREEPOCH_H
#define VOLITION_VERSIONEDSTOREEPOCH_H

#include <volition/common.h>
#include <volition/ValueStack.h>

namespace Volition {

class VersionedStore;

//================================================================//
// VersionedStoreEpoch
//================================================================//
class VersionedStoreEpoch :
    public enable_shared_from_this < VersionedStoreEpoch > {
private:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreIterator;
    template < typename > friend class VersionedValueIterator;

    typedef set < string > EpochLayer;

    set < VersionedStore* >                                 mClients;
    set < VersionedStoreEpoch* >                            mChildren;
    map < size_t, EpochLayer >                              mEpochLayers;
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;

    shared_ptr < VersionedStoreEpoch >                      mParent;
    size_t                                                  mBaseVersion;
    size_t                                                  mTopVersion;

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
    void                            affirmChild                 ( VersionedStoreEpoch& child );
    void                            affirmClient                ( VersionedStore& client );
    size_t                          countDependencies           () const;
    void                            eraseChild                  ( VersionedStoreEpoch& child );
    void                            eraseClient                 ( VersionedStore& client );
    size_t                          findImmutableTop            ( const VersionedStore* ignore = NULL ) const;
    const AbstractValueStack*       findValueStack              ( string key ) const;
    const void*                     getRaw                      ( size_t version, string key, size_t typeID ) const;
    void                            optimize                    ();
    void                            popLayer                    ();
    void                            setParent                   ( shared_ptr < VersionedStoreEpoch > parent );
    void                            setRaw                      ( size_t version, string key, const void* value );

public:

    //----------------------------------------------------------------//
                                    VersionedStoreEpoch         ();
                                    VersionedStoreEpoch         ( shared_ptr < VersionedStoreEpoch > parent, size_t baseVersion );
                                    ~VersionedStoreEpoch        ();
};

} // namespace Volition
#endif
