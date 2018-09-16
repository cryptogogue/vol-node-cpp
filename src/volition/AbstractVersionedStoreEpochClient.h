// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTVERSIONEDSTOREEPOCHCLIENT_H
#define VOLITION_ABSTRACTVERSIONEDSTOREEPOCHCLIENT_H

#include <volition/common.h>
#include <volition/ValueStack.h>

namespace Volition {

class VersionedStore;
class VersionedStoreEpoch;

//================================================================//
// AbstractVersionedStoreEpochClient
//================================================================//
class AbstractVersionedStoreEpochClient {
protected:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStoreEpoch;
    friend class VersionedStoreIterator;

    //----------------------------------------------------------------//
    virtual size_t      AbstractVersionedStoreEpochClient_getVersion    () const = 0;

public:

    //----------------------------------------------------------------//
                        AbstractVersionedStoreEpochClient               ();
    virtual             ~AbstractVersionedStoreEpochClient              ();
    size_t              getVersion                                      () const;
};

} // namespace Volition
#endif
