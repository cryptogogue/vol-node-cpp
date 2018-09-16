// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTOREITERATOR_H
#define VOLITION_VERSIONEDSTOREITERATOR_H

#include <volition/common.h>
#include <volition/VersionedStore.h>

namespace Volition {

class AbstractValueStack;

//================================================================//
// VersionedStoreIterator
//================================================================//
class VersionedStoreIterator :
    public VersionedStore {
protected:

    VersionedStore&                 mAnchor;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return this->isValid ();
    }

    //----------------------------------------------------------------//
                        VersionedStoreIterator              ( VersionedStore& versionedStore );
                        VersionedStoreIterator              ( VersionedStore& versionedStore, size_t version );
    virtual             ~VersionedStoreIterator             ();
    bool                hasNext                             () const;
    bool                hasPrev                             () const;
    bool                isValid                             () const;
    void                next                                ();
    void                prev                                ();
    void                seek                                ( size_t version );
};

} // namespace Volition
#endif
