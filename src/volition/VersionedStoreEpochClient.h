// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTOREEPOCHCLIENT_H
#define VOLITION_VERSIONEDSTOREEPOCHCLIENT_H

#include <volition/common.h>
#include <volition/ValueStack.h>

namespace Volition {

class VersionedStore;
class VersionedStoreEpoch;

//================================================================//
// VersionedStoreEpochClient
//================================================================//
class VersionedStoreEpochClient {
protected:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStoreEpoch;
    friend class VersionedStoreIterator;

    shared_ptr < VersionedStoreEpoch >          mEpoch;
    size_t                                      mVersion;

    //----------------------------------------------------------------//
    void        setEpoch                        ( shared_ptr < VersionedStoreEpoch > epoch );
    void        setEpoch                        ( shared_ptr < VersionedStoreEpoch > epoch, size_t version );

public:

    //----------------------------------------------------------------//
    size_t      getVersion                      () const;
                VersionedStoreEpochClient       ();
    virtual     ~VersionedStoreEpochClient      ();
};

} // namespace Volition
#endif
