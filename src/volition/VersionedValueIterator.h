// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDVALUEITERATOR_H
#define VOLITION_VERSIONEDVALUEITERATOR_H

#include <volition/common.h>
#include <volition/AbstractVersionedValueIterator.h>
#include <volition/VersionedStore.h>

namespace Volition {

//================================================================//
// VersionedValueIterator
//================================================================//
template < typename TYPE >
class VersionedValueIterator :
    public AbstractVersionedValueIterator {
protected:

    friend class VersionedStore;

public:

    //----------------------------------------------------------------//
    const TYPE& operator * () const {
        return this->value ();
    }

    //----------------------------------------------------------------//
    const TYPE& value () const {
        return this->mCursor.template getValue < TYPE >( this->mKey );
    }
    
    //----------------------------------------------------------------//
    VersionedValueIterator ( VersionedStore& versionedStore, string key ) :
        AbstractVersionedValueIterator ( versionedStore, key, typeid ( TYPE ).hash_code ()) {
    }
};

} // namespace Volition
#endif
