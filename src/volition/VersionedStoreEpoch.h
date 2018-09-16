// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTOREEPOCH_H
#define VOLITION_VERSIONEDSTOREEPOCH_H

#include <volition/common.h>
#include <volition/ValueStack.h>
#include <volition/VersionedStoreEpochClient.h>

namespace Volition {

class VersionedStore;
class VersionedStoreEpoch;

//================================================================//
// VersionedStoreDownstream
//================================================================//
class VersionedStoreDownstream {
private:

    friend class VersionedStore;
    friend class VersionedStoreEpoch;

    size_t      mPeers;
    size_t      mDependents;
    size_t      mTotal;
};

//================================================================//
// VersionedStoreEpoch
//================================================================//
class VersionedStoreEpoch :
    public VersionedStoreEpochClient {
private:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreEpochClient;
    friend class VersionedStoreIterator;

    typedef set < string > Layer;

    set < VersionedStoreEpochClient* >                      mClients;
    vector < unique_ptr < Layer >>                          mLayers;
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;

    //----------------------------------------------------------------//
    template < typename TYPE >
    void affirmValueStack ( string key ) {
        unique_ptr < AbstractValueStack >& versionedValue = this->mValueStacksByKey [ key ];
        if ( !versionedValue ) {
            versionedValue = make_unique < ValueStack < TYPE >>();
        }
    }

    //----------------------------------------------------------------//
    bool                                    containsVersion             ( size_t version ) const;
    size_t                                  countClients                () const;
    VersionedStoreDownstream                countDownstream             ( size_t version ) const;
    size_t                                  countLayers                 () const;
    const AbstractValueStack*               findValueStack              ( string key ) const;
    const AbstractValueStack*               findValueStack              ( string key, size_t version ) const;
    shared_ptr < VersionedStoreEpoch >      getParent                   ();
    void                                    optimize                    ();
    void                                    popLayer                    ();
    void                                    pushLayer                   ();

public:

    //----------------------------------------------------------------//
                            VersionedStoreEpoch         ();
                            VersionedStoreEpoch         ( shared_ptr < VersionedStoreEpoch > parent, size_t version );
                            ~VersionedStoreEpoch        ();
};

} // namespace Volition
#endif
