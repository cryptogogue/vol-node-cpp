// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDVALUEITERATOR_H
#define VOLITION_VERSIONEDVALUEITERATOR_H

#include <volition/common.h>
#include <volition/VersionedStoreSnapshot.h>

namespace Volition {

//================================================================//
// VersionedValueIterator
//================================================================//
template < typename TYPE >
class VersionedValueIterator :
    public VersionedStoreSnapshot {
protected:

    friend class VersionedStoreSnapshot;

    typedef typename map < size_t, TYPE >::const_iterator ValueIterator;

    enum {
        VALID,
        
        // these are only set *after* a call to prev() or next().
        // they are not meant to be exposed or for general use.
        NO_PREV,
        NO_NEXT,
    };

    VersionedStoreSnapshot      mAnchor;
    string                      mKey;
    ValueIterator               mIterator;
    int                         mState;
    
    size_t                      mFirstVersion;
    size_t                      mLastVersion;

    //----------------------------------------------------------------//
    const ValueStack < TYPE >* getValueStack () {
        
        return this->getValueStack ( this->mBranch );
    }

    //----------------------------------------------------------------//
    const ValueStack < TYPE >* getValueStack ( shared_ptr < VersionedStoreBranch > branch ) {
    
        if ( branch ) {
            const AbstractValueStack* abstractValueStack = branch->findValueStack ( this->mKey );
            if ( abstractValueStack ) {
                return dynamic_cast < const ValueStack < TYPE >* >( abstractValueStack );
            }
        }
        return NULL;
    }

    //----------------------------------------------------------------//
    void seekNext ( shared_ptr < VersionedStoreBranch > prevBranch ) {
        
        shared_ptr < VersionedStoreBranch > branch = this->mAnchor.mBranch;
        size_t top = this->mAnchor.mVersion + 1;
        
        shared_ptr < VersionedStoreBranch > bestBranch;
        const ValueStack < TYPE >* bestValueStack = NULL;
        size_t bestTop = top;
        
        for ( ; branch != prevBranch; branch = branch->mBranch ) {
            const ValueStack < TYPE >* valueStack = this->getValueStack ( branch );
            if ( valueStack && valueStack->size ()) {
                bestBranch = branch;
                bestValueStack = valueStack;
                bestTop = top;
            }
            top = branch->mBaseVersion;
        }
        
        if ( bestValueStack ) {
            this->setExtents ( *bestValueStack, bestTop - 1 );
            this->mIterator = bestValueStack->mValuesByVersion.begin ();
            this->setBranch ( bestBranch, this->mFirstVersion );
            this->mState = VALID;
        }
        else {
            this->mState = NO_NEXT;
        }
    }

    //----------------------------------------------------------------//
    void seekPrev ( shared_ptr < VersionedStoreBranch > branch, size_t top ) {
        
        for ( ; branch; branch = branch->mBranch ) {
        
            const ValueStack < TYPE >* valueStack = this->getValueStack ( branch );
            
            if ( valueStack && valueStack->size ()) {
                this->setExtents ( *valueStack, top - 1 );
                this->mIterator = valueStack->mValuesByVersion.find ( this->mLastVersion );
                this->setBranch ( branch, this->mLastVersion );
                this->mState = VALID;
                return;
            }
            top = branch->mBaseVersion;
        }
        this->mState = NO_PREV;
    }

    //----------------------------------------------------------------//
    void setExtents ( const ValueStack < TYPE >& valueStack, size_t top ) {
    
        assert ( valueStack.mValuesByVersion.size ());
        this->mFirstVersion     = valueStack.mValuesByVersion.begin ()->first;
        
        typename map < size_t, TYPE >::const_iterator valueIt = valueStack.mValuesByVersion.lower_bound ( top );
        
        if ( valueIt == valueStack.mValuesByVersion.cend ()) {
            this->mLastVersion = valueStack.mValuesByVersion.rbegin ()->first;
        }
        else {
            if ( valueIt->first > top ) {
                valueIt--;
            }
            this->mLastVersion = valueIt->first;
        }
    }

public:

    //----------------------------------------------------------------//
    operator bool () const {
        return this->isValid ();
    }

    //----------------------------------------------------------------//
    const TYPE& operator * () const {
        return this->value ();
    }

    //----------------------------------------------------------------//
    bool isValid () const {
        return ( this->mState == VALID );
    }
    
    //----------------------------------------------------------------//
    bool next () {
        
        if ( !this->mBranch ) return false;
        
        if ( this->mState == NO_PREV ) {
            this->mState = VALID;
        }
        else if ( this->mState != NO_NEXT ) {
            
             if ( this->mIterator->first < this->mLastVersion ) {
                ++this->mIterator;
                this->mVersion = this->mIterator->first;
            }
            else {
                assert ( this->mIterator->first == this->mLastVersion );
                this->seekNext ( this->mBranch );
            }
        }
        return ( this->mState != NO_NEXT );
    }
    
    //----------------------------------------------------------------//
    bool prev () {

        if ( !this->mBranch ) return false;

        if ( this->mState == NO_NEXT ) {
            this->mState = VALID;
        }
        else if ( this->mState != NO_PREV ) {
                
            if ( this->mIterator->first > this->mFirstVersion ) {
                --this->mIterator;
                this->mVersion = this->mIterator->first;
            }
            else {
                assert ( this->mIterator->first == this->mFirstVersion );
                this->seekPrev ( this->mBranch->mBranch, this->mVersion );
            }
        }
        return ( this->mState != NO_PREV );
    }

    //----------------------------------------------------------------//
    const TYPE& value () const {
        assert ( this->isValid ());
        return this->mIterator->second;
    }
    
    //----------------------------------------------------------------//
    VersionedValueIterator ( VersionedStoreSnapshot& versionedStore, string key ) :
        mAnchor ( versionedStore ),
        mKey ( key ) {
        
        if ( this->mAnchor.mBranch ) {
            this->seekPrev ( this->mAnchor.mBranch, this->mAnchor.mVersion + 1 );
        }
    }
};

} // namespace Volition
#endif
