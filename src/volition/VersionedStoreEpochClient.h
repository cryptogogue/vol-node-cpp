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

    friend class VersionedStoreEpoch;

    shared_ptr < VersionedStoreEpoch >          mEpoch;
    size_t                                      mVersion;

public:

    //----------------------------------------------------------------//
    size_t      getVersion                      () const;
    void        setEpoch                        ( shared_ptr < VersionedStoreEpoch > epoch );
                VersionedStoreEpochClient       ();
    virtual     ~VersionedStoreEpochClient      ();
};

} // namespace Volition
#endif
