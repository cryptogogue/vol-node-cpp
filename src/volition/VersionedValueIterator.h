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
/** \brief Implements a sparse version iterator of a specific value
    in a store.

    The iterator can traverse from the current version of the value
    stack used to initialize it back to version zero. Iteration works
    in both directions, though iterating forward may incur additional
    overhead in cases where multiple branches exist.
 
    When the iterator is initialized, an "anchor" snapshot is created.
    This ensures that the values being iterated will not change, even
    if the initializing snapshot is rewound and modified. In other words,
    the iterator is itself a snapshot of the entire version history.
 
    If the iterator is iterated past the beginning or end of its
    range, it will become invalid. To restore it, step the iterator
    in the opposite direction.
 
    Iteration is performed in a sparse manner: only the versions of
    the store where the value was modified will be visited.
 
    \todo The branch's internal direct reference counter should be used
    to prevent branch optimization during iteration.
*/
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

    /// The anchor snapshot.
    VersionedStoreSnapshot      mAnchor;
    
    /// Key of the value being iterated.
    string                      mKey;
    
    /// Internal iterator of undersling value stack.
    ValueIterator               mIterator;
    
    /// Internal state of the iterator.
    int                         mState;
    
    /// Lower bound of the stack being iterated.
    size_t                      mFirstVersion;
    
    /// Upper bound of the stack being iterated.
    size_t                      mLastVersion;

    //----------------------------------------------------------------//
    /** \brief Calls getValueStack() on the branch currently being iterated.
     
        \return     A pointer to the strongly typed ValueStack or NULL.
    */
    const ValueStack < TYPE >* getValueStack () {
        
        return this->getValueStack ( this->mBranch );
    }

    //----------------------------------------------------------------//
    /** \brief Helper method to find an AbstractValueStack in a branch and
        dynamically cast it to a strongly typed implementation. Uses the intenal
        key to find the value stack.
     
        \return     A pointer to the strongly typed ValueStack or NULL.
    */
    const ValueStack < TYPE >* getValueStack ( shared_ptr < VersionedBranch > branch ) {
    
        if ( branch ) {
            const AbstractValueStack* abstractValueStack = branch->findValueStack ( this->mKey );
            if ( abstractValueStack ) {
                return dynamic_cast < const ValueStack < TYPE >* >( abstractValueStack );
            }
        }
        return NULL;
    }

    //----------------------------------------------------------------//
    /** \brief Starts at the anchor's branch and seeks backward to find the
        "next" value. This is called when the upper bound of a branch is reached.
     
        \param     prevBranch   Lower bound branch; the seek will stop at (and exclude) this branch.
    */
    void seekNext ( shared_ptr < VersionedBranch > prevBranch ) {
        
        shared_ptr < VersionedBranch > branch = this->mAnchor.mSourceBranch;
        size_t top = this->mAnchor.mVersion + 1;
        
        shared_ptr < VersionedBranch > bestBranch;
        const ValueStack < TYPE >* bestValueStack = NULL;
        size_t bestTop = top;
        
        for ( ; branch != prevBranch; branch = branch->mSourceBranch ) {
            const ValueStack < TYPE >* valueStack = this->getValueStack ( branch );
            if ( valueStack && valueStack->size ()) {
                bestBranch = branch;
                bestValueStack = valueStack;
                bestTop = top;
            }
            top = branch->mVersion;
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
    /** \brief Starts at the current branch and seeks backward to find the
        "prev" value. This is called when the lower bound of a branch is reached.
     
        An "upper bound" is given via the "top" param. This is necessary to correctly
        set the branch bounds.
     
        \param      branch      Starting branch for the search.
        \param      top         Upper bound for the search.
    */
    void seekPrev ( shared_ptr < VersionedBranch > branch, size_t top ) {
        
        for ( ; branch; branch = branch->mSourceBranch ) {
        
            const ValueStack < TYPE >* valueStack = this->getValueStack ( branch );
            
            if ( valueStack && valueStack->size ()) {
                this->setExtents ( *valueStack, top - 1 );
                this->mIterator = valueStack->mValuesByVersion.find ( this->mLastVersion );
                this->setBranch ( branch, this->mLastVersion );
                this->mState = VALID;
                return;
            }
            top = branch->mVersion;
        }
        this->mState = NO_PREV;
    }

    //----------------------------------------------------------------//
    /** \brief Updates the internal bounds of the stack.
    
        The upper bound must be given; it cannot be deduced from the stack
        alone (as the stack may be shared by other branches with higher base
        versions).
     
        The lower bound is simply the lowest version held in the stack.
     
        \param      valueStack      The stack to be used to setting the bounds. Must at least one value.
        \param      top             Upper bound for the stack.
    */
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
    /** \brief Operator overload for bool. False if the iterator is empty
        or has been iterated past its bounds. Calls isValid() internally.
    */
    operator bool () const {
        return this->isValid ();
    }

    //----------------------------------------------------------------//
    /** \brief Prefix increment operator. Calls next() internally.
    */
    VersionedStoreIterator& operator++ () {
        this->next ();
        return *this;
    }
    
    //----------------------------------------------------------------//
    /** \brief Prefix decrement operator. Calls prev() internally.
    */
    VersionedStoreIterator& operator-- () {
        this->prev ();
        return *this;
    }

    //----------------------------------------------------------------//
    /** \brief Dereference operator. Calls value() internally.
    */
    const TYPE& operator * () const {
        return this->value ();
    }

    //----------------------------------------------------------------//
    /** \brief Returns true if iterator is in the valid range for iteration.

        \return     True if iteration is in the valid range.
    */
    bool isValid () const {
        return ( this->mState == VALID );
    }
    
    //----------------------------------------------------------------//
    /** \brief Iterates forward by one version.

        \return     True if the step was successful.
    */
    bool next () {
        
        if ( !this->mSourceBranch ) return false;
        
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
                this->seekNext ( this->mSourceBranch );
            }
        }
        return ( this->mState != NO_NEXT );
    }
    
    //----------------------------------------------------------------//
    /** \brief Iterates backward by one version.

        \return     True if the step was successful.
    */
    bool prev () {

        if ( !this->mSourceBranch ) return false;

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
                this->seekPrev ( this->mSourceBranch->mSourceBranch, this->mVersion );
            }
        }
        return ( this->mState != NO_PREV );
    }

    //----------------------------------------------------------------//
    /** \brief Returns a const reference to the current value being iterated.

        \return     The current value of the iterator.
    */
    const TYPE& value () const {
        assert ( this->isValid ());
        return this->mIterator->second;
    }
    
    //----------------------------------------------------------------//
    /** \brief Initialize the iterator with a given snapshot and set the
        key of the value to iterate. Internal type of the key must match the
        template type.

        \param  versionedStore  Snapshot to use as the upper bound for iteration.
        \param  key             Key of the value to be iterated.
    */
    VersionedValueIterator ( VersionedStoreSnapshot& versionedStore, string key ) :
        mAnchor ( versionedStore ),
        mKey ( key ) {
        
        if ( this->mAnchor.mSourceBranch ) {
            this->seekPrev ( this->mAnchor.mSourceBranch, this->mAnchor.mVersion + 1 );
        }
    }
};

} // namespace Volition
#endif
