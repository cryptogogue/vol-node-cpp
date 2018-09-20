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

    if ( this->mAnchor.mEpoch && ( this->mAnchor.mEpoch->mTopVersion > 0 )) {
        this->takeSnapshot ( versionedStore );
    }
    this->mState = this->mEpoch ? VALID : EMPTY;
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
bool VersionedStoreIterator::isValid () const {

    return ( this->mState == VALID );
}

//----------------------------------------------------------------//
bool VersionedStoreIterator::next () {

    if ( this->mState == EMPTY ) return false;

    if ( this->mState == NO_PREV ) {
        this->mState = VALID;
    }
    else if ( this->mState != NO_NEXT ) {

        if ( this->mVersion < this->mAnchor.mVersion ) {
            this->seek ( this->mVersion + 1 );
        }
        else {
            this->mState = NO_NEXT;
        }
    }
    return ( this->mState != NO_NEXT );
}

//----------------------------------------------------------------//
bool VersionedStoreIterator::prev () {

    if ( this->mState == EMPTY ) return false;

    if ( this->mState == NO_NEXT ) {
        this->mState = VALID;
    }
    else if ( this->mState != NO_PREV ) {

        if ( this->mVersion > 0 ) {
            this->seek ( this->mVersion - 1 );
        }
        else {
            this->mState = NO_PREV;
        }
    }
    return ( this->mState != NO_PREV );
}

//----------------------------------------------------------------//
void VersionedStoreIterator::seek ( size_t version ) {

    if ( this->mState == EMPTY ) return;

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
    this->mState = VALID;
}

} // namespace Volition
