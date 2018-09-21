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

    if ( this->mAnchor.mEpoch && ( this->mAnchor.mEpoch->getTopVersion () > 0 )) {
        this->takeSnapshot ( versionedStore );
        this->mTopVersion = this->mVersion + 1;
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
    
        while ( epoch && !(( epoch->mBaseVersion <= version ) && ( version <= this->mTopVersion ))) {
            this->mTopVersion = epoch->mBaseVersion - 1;
            epoch = epoch->mParent;
        }
        assert ( epoch );
        this->setEpoch ( epoch, version );
    }
    else if ( version > this->mVersion ) {
    
        this->mVersion = version;
    
        if ( this->mVersion > this->mTopVersion ) {
            
            this->setEpoch ( this->mAnchor.mEpoch, this->mAnchor.mVersion ); // overwrites this->mVersion
            this->mTopVersion = this->mVersion;
            
            if ( version < this->mVersion ) {
                this->seek ( version );
            }
        }
    }
    this->mState = VALID;
}

} // namespace Volition
