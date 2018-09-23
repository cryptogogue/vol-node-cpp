// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTOREITERATOR_H
#define VOLITION_VERSIONEDSTOREITERATOR_H

#include <volition/common.h>
#include <volition/VersionedStoreBranchClient.h>

namespace Volition {

class AbstractValueStack;

//================================================================//
// VersionedStoreIterator
//================================================================//
class VersionedStoreIterator :
    public VersionedStoreBranchClient {
protected:

    enum {
        VALID,
        EMPTY,
        
        // these are only set *after* a call to prev() or next().
        // they are not meant to be exposed or for general use.
        NO_PREV,
        NO_NEXT,
    };

    VersionedStoreBranchClient  mAnchor;
    int                         mState;
    size_t                      mTopVersion;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return this->isValid ();
    }

    //----------------------------------------------------------------//
                        VersionedStoreIterator              ( VersionedStoreBranchClient& versionedStore );
                        VersionedStoreIterator              ( VersionedStoreBranchClient& versionedStore, size_t version );
    virtual             ~VersionedStoreIterator             ();
    bool                isValid                             () const;
    bool                next                                ();
    bool                prev                                ();
    void                seek                                ( size_t version );
};

} // namespace Volition
#endif
