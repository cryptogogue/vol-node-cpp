// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTOREEPOCH_H
#define VOLITION_VERSIONEDSTOREEPOCH_H

#include <volition/common.h>
#include <volition/AbstractVersionedStoreEpochClient.h>
#include <volition/ValueStack.h>

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
    public AbstractVersionedStoreEpochClient {
private:

    friend class AbstractVersionedStoreEpochClient;
    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreIterator;

    typedef set < string > Layer;

    set < AbstractVersionedStoreEpochClient* >              mClients;
    vector < unique_ptr < Layer >>                          mLayers;
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;

    shared_ptr < VersionedStoreEpoch >                      mParent;
    size_t                                                  mBaseVersion;

    //----------------------------------------------------------------//
    template < typename TYPE >
    void affirmValueStack ( string key ) {
        unique_ptr < AbstractValueStack >& versionedValue = this->mValueStacksByKey [ key ];
        if ( !versionedValue ) {
            versionedValue = make_unique < ValueStack < TYPE >>();
        }
    }

    //----------------------------------------------------------------//
    void                            affirmClient                ( AbstractVersionedStoreEpochClient& client );
    bool                            containsVersion             ( size_t version ) const;
    size_t                          countClients                () const;
    VersionedStoreDownstream        countDownstream             ( size_t version ) const;
    size_t                          countLayers                 () const;
    void                            eraseClient                 ( AbstractVersionedStoreEpochClient& client );
    const AbstractValueStack*       findValueStack              ( string key ) const;
    const AbstractValueStack*       findValueStack              ( string key, size_t version ) const;
    void                            optimize                    ();
    void                            popLayer                    ();
    void                            pushLayer                   ();
    void                            setParent                   ( shared_ptr < VersionedStoreEpoch > parent, size_t baseVersion );

    //----------------------------------------------------------------//
    size_t                          AbstractVersionedStoreEpochClient_getVersion    () const override;

public:

    //----------------------------------------------------------------//
                            VersionedStoreEpoch         ();
                            VersionedStoreEpoch         ( shared_ptr < VersionedStoreEpoch > parent, size_t version );
                            ~VersionedStoreEpoch        ();
};

} // namespace Volition
#endif
