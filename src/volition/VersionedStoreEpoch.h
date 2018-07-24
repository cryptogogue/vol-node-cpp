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

    friend class VersionedStore;

    typedef set < string > Layer;

    set < VersionedStore* >                                 mClients;
    vector < unique_ptr < Layer >>                          mLayers;
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;
    
    shared_ptr < VersionedStoreEpoch >                      mParent;
    set < VersionedStoreEpoch* >                            mChildren;

    //----------------------------------------------------------------//
    template < typename TYPE >
    void affirmValueStack ( string key ) {
        unique_ptr < AbstractValueStack >& versionedValue = this->mValueStacksByKey [ key ];
        if ( !versionedValue ) {
            versionedValue = make_unique < ValueStack < TYPE >>();
        }
    }

    //----------------------------------------------------------------//
    size_t                                  countChildren               () const;
    size_t                                  countClients                () const;
    size_t                                  countLayers                 () const;
    void                                    copyBackLayerToFront        ( VersionedStoreEpoch& epoch ) const;
    const AbstractValueStack*               findValueStack              ( string key ) const;
    shared_ptr < VersionedStoreEpoch >      getOnlyChild                ();
    VersionedStore*                         getOnlyClient               ();
    shared_ptr < VersionedStoreEpoch >      getParent                   ();
    void                                    moveChildrenTo              ( VersionedStoreEpoch& epoch );
    void                                    moveClientTo                ( VersionedStore& client, shared_ptr < VersionedStoreEpoch > epoch );
    void                                    moveClientsTo               ( VersionedStoreEpoch& epoch, const VersionedStore* except = NULL );
    void                                    popLayer                    ();
    void                                    pushLayer                   ();
    void                                    setParent                   ( shared_ptr < VersionedStoreEpoch > parent );

public:

    //----------------------------------------------------------------//
                                            VersionedStoreEpoch         ();
                                            ~VersionedStoreEpoch        ();
};

} // namespace Volition
#endif
