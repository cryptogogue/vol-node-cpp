// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTVERSIONEDVALUEITERATOR_H
#define VOLITION_ABSTRACTVERSIONEDVALUEITERATOR_H

#include <volition/common.h>
#include <volition/VersionedStore.h>

namespace Volition {

class AbstractValueStack;

//================================================================//
// AbstractVersionedValueIterator
//================================================================//
class AbstractVersionedValueIterator {
protected:

    friend class VersionedStore;

    enum {
        VALID,
        
        // these are only set *after* a call to prev() or next().
        // they are not meant to be exposed or for general use.
        NO_PREV,
        NO_NEXT,
    };

    VersionedStore&                 mAnchor;
    VersionedStore                  mCursor;
    string                          mKey;
    
    const AbstractValueStack*       mValueStack;
    size_t                          mIndex;
    int                             mState;

    //----------------------------------------------------------------//
    void                seekPrev                                    ( shared_ptr < VersionedStoreEpoch > epoch, size_t version );

    //----------------------------------------------------------------//
                        AbstractVersionedValueIterator              ( VersionedStore& versionedStore, string key, size_t typeID );

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return this->isValid ();
    }

    //----------------------------------------------------------------//
    virtual             ~AbstractVersionedValueIterator             ();
    bool                isValid                                     () const;
    void                next                                        ();
    void                prev                                        ();
    size_t              version                                     () const;
};

} // namespace Volition
#endif
