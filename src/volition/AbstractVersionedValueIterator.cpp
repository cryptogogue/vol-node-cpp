// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/AbstractValueStack.h>
#include <volition/AbstractVersionedValueIterator.h>

namespace Volition {


//================================================================//
// AbstractVersionedValueIterator
//================================================================//

//----------------------------------------------------------------//
AbstractVersionedValueIterator::AbstractVersionedValueIterator ( VersionedStore& versionedStore, string key, size_t typeID ) :
    mAnchor ( versionedStore ),
    mKey ( key ) {

    this->seekPrev ( versionedStore.mEpoch, versionedStore.getVersion ());
    
    assert ( this->mValueStack );
    assert ( this->mValueStack->mTypeID == typeID );
}

//----------------------------------------------------------------//
AbstractVersionedValueIterator::~AbstractVersionedValueIterator () {
}

//----------------------------------------------------------------//
bool AbstractVersionedValueIterator::isValid () const {

    return ( this->mState == VALID );
}

//----------------------------------------------------------------//
void AbstractVersionedValueIterator::next () {

    switch ( this->mState ) {

        case VALID: {

            if (( this->mIndex + 1 ) < this->mValueStack->size ()) {
                this->mIndex++;
                this->mCursor.mVersion = this->mValueStack->getVersionForIndex ( this->mIndex );
            }
            else {
            
                size_t version = this->mCursor.mVersion;
            
                shared_ptr < VersionedStoreEpoch > epochCursor = this->mAnchor.mEpoch;
                shared_ptr < VersionedStoreEpoch > epoch = NULL;
                const AbstractValueStack* valueStack = NULL;
                
                for ( ; epochCursor && ( epochCursor->mVersion > version ); epochCursor = epochCursor->getParent ()) {
                    valueStack = epochCursor->findValueStack ( this->mKey );
                    epoch = epochCursor;
                }

                if ( valueStack ) {
                    this->mValueStack = valueStack;
                    this->mIndex = 0;
                    this->mCursor.setEpoch ( epoch, this->mValueStack->getVersionForIndex ( 0 ));
                }
                else {
                    this->mState = NO_NEXT;
                }
            }
            break;
        }
        
         case NO_PREV: {
            this->mState = VALID;
            break;
        }
    }
}

//----------------------------------------------------------------//
void AbstractVersionedValueIterator::prev () {

    switch ( this->mState ) {

        case VALID: {

            if ( this->mIndex > 0 ) {
                this->mIndex--;
                this->mCursor.mVersion = this->mValueStack->getVersionForIndex ( this->mIndex );
            }
            else {
                this->seekPrev ( this->mCursor.mEpoch->getParent (), this->mCursor.mEpoch->getVersion () - 1 );
            }
            break;
        }
        
        case NO_NEXT: {
            this->mState = VALID;
            break;
        }
    }
}

//----------------------------------------------------------------//
void AbstractVersionedValueIterator::seekPrev ( shared_ptr < VersionedStoreEpoch > epoch, size_t version ) {

    const AbstractValueStack* valueStack = NULL;
    for ( ; epoch; epoch = epoch->getParent ()) {
    
        assert ( epoch->getVersion () <= version );
    
        valueStack = epoch->findValueStack ( this->mKey );
        
        for ( size_t i = valueStack->size (); i > 0; --i ) {
        
            size_t index = i - 1;
            size_t prevVersion = valueStack->getVersionForIndex ( index );
            
            if ( prevVersion <= version ) {
            
                this->mValueStack = valueStack;
                this->mIndex = index;
                this->mCursor.setEpoch ( epoch, prevVersion );
                this->mState = VALID;
                return;
            }
        }
    }
    this->mState = NO_PREV;
}

//----------------------------------------------------------------//
size_t AbstractVersionedValueIterator::version () const {

    return this->mValueStack->getVersionForIndex ( this->mIndex );
}

} // namespace Volition
