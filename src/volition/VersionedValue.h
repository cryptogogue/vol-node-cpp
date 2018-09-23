// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDVALUE_H
#define VOLITION_VERSIONEDVALUE_H

#include <volition/common.h>
#include <volition/VersionedStoreBranchClient.h>

namespace Volition {

//================================================================//
// VersionedValue
//================================================================//
template < typename TYPE >
class VersionedValue {
protected:

    VersionedStoreBranchClient  mAnchor;
    const TYPE*                 mValue;

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return ( this->mValue != NULL );
    }

    //----------------------------------------------------------------//
    operator const TYPE* () const {
        return this->mValue;
    }

    //----------------------------------------------------------------//
    const TYPE* operator-> () {
        return this->mValue;
    }

    //----------------------------------------------------------------//
    const TYPE& operator * () const {
        assert ( this->mValue );
        return *this->mValue;
    }

    //----------------------------------------------------------------//
    VersionedValue < TYPE >& operator = ( const VersionedValue < TYPE >& other ) {
        this->mAnchor.takeSnapshot ( const_cast < VersionedValue < TYPE >& >( other ).mAnchor );
        this->mValue = other.mValue;
        return *this;
    }

    //----------------------------------------------------------------//
    void setValue ( const VersionedStoreBranchClient& versionedStore, string key ) {
        
        this->mValue = versionedStore.getValueOrNil < TYPE >( key, versionedStore.getVersion ());
        if ( this->mValue ) {
            this->mAnchor.takeSnapshot ( const_cast < VersionedStoreBranchClient& >( versionedStore ));
        }
        else {
            this->mAnchor.clear ();
        }
    }

    //----------------------------------------------------------------//
//    const TYPE& value () const {
//        assert ( this->mValue );
//        return *this->mValue;
//    }
    
    //----------------------------------------------------------------//
    VersionedValue () :
        mValue ( NULL ) {
    }
    
    //----------------------------------------------------------------//
    VersionedValue ( const VersionedValue < TYPE >& other ) {
        this->mAnchor.takeSnapshot ( const_cast < VersionedValue < TYPE >& >( other ).mAnchor );
        this->mValue = other.mValue;
    }
    
    //----------------------------------------------------------------//
    VersionedValue ( const VersionedStoreBranchClient& versionedStore, string key ) {
        this->setValue ( versionedStore, key );
    }
};

} // namespace Volition
#endif
