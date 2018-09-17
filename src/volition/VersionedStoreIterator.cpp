// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStoreIterator.h>

namespace Volition {

//================================================================//
// VersionedStoreIterator
//================================================================//

//----------------------------------------------------------------//
VersionedStoreIterator::VersionedStoreIterator ( VersionedStore& versionedStore ) :
    mAnchor ( versionedStore ) {

    this->takeSnapshot ( versionedStore );
}

//----------------------------------------------------------------//
VersionedStoreIterator::VersionedStoreIterator ( VersionedStore& versionedStore, size_t version ) :
    VersionedStoreIterator ( versionedStore ) {

    this->seek ( version );
}

//----------------------------------------------------------------//
VersionedStoreIterator::~VersionedStoreIterator () {
}

//----------------------------------------------------------------//
bool VersionedStoreIterator::hasNext () const {

    return ( this->mVersion < this->mAnchor.mVersion );
}

//----------------------------------------------------------------//
bool VersionedStoreIterator::hasPrev () const {

    return ( this->mVersion > 0 );
}

//----------------------------------------------------------------//
void VersionedStoreIterator::next () {

    if ( this->hasNext ()) {
        this->seek ( this->mVersion + 1 );
    }
}

//----------------------------------------------------------------//
void VersionedStoreIterator::prev () {

    if ( this->hasPrev ()) {
        this->seek ( this->mVersion - 1 );
    }
}

//----------------------------------------------------------------//
void VersionedStoreIterator::seek ( size_t version ) {

    if ( version < this->mVersion ) {
    
        shared_ptr < VersionedStoreEpoch > epoch = this->mEpoch;
        size_t top = this->mVersion;
    
        while ( epoch && !(( epoch->mBaseVersion <= version ) && ( version < top ))) {
            top = epoch->mBaseVersion;
            epoch = epoch->mParent;
        }
        assert ( epoch );
        this->setEpoch ( epoch, version );
        
    }
    else if ( version > this->mVersion ) {
    
        if ( version > this->mAnchor.mVersion ) {
            version = this->mAnchor.mVersion;
        }
        
        this->setEpoch ( this->mAnchor.mEpoch, this->mAnchor.mVersion );
        if ( version < this->mVersion ) {
            this->seek ( version );
        }
    }
}

} // namespace Volition
