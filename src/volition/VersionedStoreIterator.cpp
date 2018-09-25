// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStoreIterator.h>

namespace Volition {

//================================================================//
// VersionedStoreIterator
//================================================================//

//----------------------------------------------------------------//
/** \brief Returns true if iterator is in the valid range for iteration.

    \return     True if iteration is in the valid range.
*/
bool VersionedStoreIterator::isValid () const {

    return ( this->mState == VALID );
}

//----------------------------------------------------------------//
/** \brief Iterates forward by one version.

    \return     True if the step was successful.
*/
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
/** \brief Iterates backward by one version.

    \return     True if the step was successful.
*/
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
/** \brief Seeks forward or backward in the range.

    \param  version     Version to seek to.
*/
void VersionedStoreIterator::seek ( size_t version ) {

    if ( this->mState == EMPTY ) return;

    if ( version < this->mVersion ) {
    
        shared_ptr < VersionedBranch > branch = this->mSourceBranch;
    
        while ( branch && !(( branch->mVersion <= version ) && ( version <= this->mTopVersion ))) {
            this->mTopVersion = branch->mVersion - 1;
            branch = branch->mSourceBranch;
        }
        assert ( branch );
        this->setBranch ( branch, version );
    }
    else if ( version > this->mVersion ) {
    
        this->mVersion = version;
    
        if ( this->mVersion > this->mTopVersion ) {
            
            this->setBranch ( this->mAnchor.mSourceBranch, this->mAnchor.mVersion ); // overwrites this->mVersion
            this->mTopVersion = this->mVersion;
            
            if ( version < this->mVersion ) {
                this->seek ( version );
            }
        }
    }
    this->mState = VALID;
}

//----------------------------------------------------------------//
/** \brief Initialize the iterator with a given snapshot. Upper bound for
    iteration is the current version of the snapshot.

    \param  versionedStore  Snapshot to use as the upper bound for iteration.
*/
VersionedStoreIterator::VersionedStoreIterator ( VersionedStoreSnapshot& versionedStore ) :
    mAnchor ( versionedStore ) {

    if ( this->mAnchor.mSourceBranch && ( this->mAnchor.mSourceBranch->getTopVersion () > 0 )) {
        this->takeSnapshot ( versionedStore );
        this->mTopVersion = this->mVersion + 1;
    }
    this->mState = this->mSourceBranch ? VALID : EMPTY;
}

//----------------------------------------------------------------//
/** \brief Initialize the iterator with a given snapshot and seek to
    a specific version. Upper bound for iteration is the current version
    of the snapshot.

    \param  versionedStore  Snapshot to use as the upper bound for iteration.
    \param  version         Version to seek back to. Must be equal to or less than the upper bound.
*/
VersionedStoreIterator::VersionedStoreIterator ( VersionedStoreSnapshot& versionedStore, size_t version ) :
    VersionedStoreIterator ( versionedStore ) {

    this->seek ( version );
}

//----------------------------------------------------------------//
VersionedStoreIterator::~VersionedStoreIterator () {
}

} // namespace Volition
