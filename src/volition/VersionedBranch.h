// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDBRANCH_H
#define VOLITION_VERSIONEDBRANCH_H

#include <volition/common.h>
#include <volition/AbstractVersionedBranchClient.h>
#include <volition/ValueStack.h>

namespace Volition {

class AbstractVersionedBranchClient;
class VersionedStoreSnapshot;

//================================================================//
// VersionedBranch
//================================================================//
/** \brief VersionedBranch is an internal data structure used to store
    a contiguous block of versioned values.
 
    Each branch is a sparse record of changes to the database over a
    span of versions. It contains a map of ValueStack instances referenced
    by key. In addition, a sparse stack of version layers (implemented as a map of
    string sets) is used to quickly identify the keys of values modified in that layer.
 
    The branch tracks a base version. Its "top" version is the highest version
    index in the layer stack. When a layer is removed, the keys of the values that were
    set in that layer are retrieved from the layer stack and used to erase the
    corresponding values from each value stack.
 
    Any branch may have a parent branch. When searching for values, the tree will
    be searched recurively until a value is found.
*/
class VersionedBranch :
    public enable_shared_from_this < VersionedBranch >,
    public AbstractVersionedBranchClient {
private:

    friend class AbstractVersionedBranchClient;
    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreSnapshot;
    friend class VersionedStoreIterator;
    template < typename > friend class VersionedValue;
    template < typename > friend class VersionedValueIterator;

    typedef set < string > Layer;

    /// Set containing active clients. This is needed to calculate dependencies and to know when to optimize.
    set < AbstractVersionedBranchClient* >                  mClients;
    
    /// Sparse array mapping versions onto layers. Each layer holds the set of keys corresponding to values that were set or modified in the version.
    map < size_t, Layer >                                   mLayers;
    
    /// Map of value stacks, indexed by key.
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;

    /// The number of clients holding direct references to branch internals. A nonzero direct reference count will prevent optimization of the branch.
    size_t                                                  mDirectReferenceCount;
    
    //----------------------------------------------------------------//
    /** \brief Recursively searches the branch to find the value for the key. The most recent version
        equal to or earlier will be returned.

        A pointer to the value or NULL is returned.

        \param      version     Search for this version of the most recent lesser version of the value;
        \param      key         The key.
        \return                 A raw pointer to the value for the key or NULL.
    */
    template < typename TYPE >
    const TYPE* getValueOrNil ( size_t version, string key ) const {

        // start searching at the current branch.
        const VersionedBranch* branch = this;
        
        // iterate through parent branches.
        for ( ; branch; branch = branch->mSourceBranch.get ()) {
        
            // ignore branches above the version we're searching for.
            if ( branch->mVersion <= version ) {
            
                // check for a value stack without recursion.
                const AbstractValueStack* abstractValueStack = branch->findValueStack ( key );
                
                if ( abstractValueStack ) {
                
                    // we can optimize this later if we need to.
                    const ValueStack < TYPE >* valueStack = dynamic_cast < const ValueStack < TYPE >* >( abstractValueStack );
                    assert ( valueStack );
                    
                    // check if value stack contains an ID for a matching or most recent lesser version.
                    // it's possible that all values are more recent than the version, in which case
                    // NULL will be returned and we'll keep searching.
                    const TYPE* value = valueStack->getValueOrNil ( version );
                    if ( value ) {
                        return value;
                    }
                }
            }
            
            // cap the version at the base version before moving to the parent branch.
            // necessary because branches don't always emerge from the top.
            version = branch->mVersion;
        }
        return NULL;
    }

    //----------------------------------------------------------------//
    /** \brief Sets a value at the given version. If the version doesn't exist,
        a new layer will be created. Also creates a value stack if none exists.
     
        \param      version     The version to set the value at. Must be equal to or greater than the branch's base version.
        \param      key         Key of the value to set.
        \param      value       Raw pointer to value to set.
    */
    template < typename TYPE >
    void setValue ( size_t version, string key, const TYPE& value ) {
        
        assert ( this->mVersion <= version );
        
        unique_ptr < AbstractValueStack >& abstractValueStack = this->mValueStacksByKey [ key ];
        
        if ( !abstractValueStack ) {
            abstractValueStack = make_unique < ValueStack < TYPE >>();
        }
        assert ( abstractValueStack );
        
        // we can optimize this later if we need to.
        ValueStack < TYPE >* valueStack = dynamic_cast < ValueStack < TYPE >* >( abstractValueStack.get ());
        assert ( valueStack );

        valueStack->setValue ( version, value );
        
        Layer& layer = this->mLayers [ version ];
        if ( layer.find ( key ) == layer.end ()) {
            layer.insert ( key );
        }
    }

    //----------------------------------------------------------------//
    size_t                          countDependencies           () const;
    void                            eraseClient                 ( AbstractVersionedBranchClient& client );
    size_t                          findImmutableTop            ( const AbstractVersionedBranchClient* ignore = NULL ) const;
    const AbstractValueStack*       findValueStack              ( string key ) const;
    shared_ptr < VersionedBranch >  fork                        ( size_t baseVersion );
    const void*                     getRaw                      ( size_t version, string key, size_t typeID ) const;
    size_t                          getTopVersion               () const;
    void                            insertClient                ( AbstractVersionedBranchClient& client );
    void                            optimize                    ();
    void                            truncate                    ( size_t topVersion );

    //----------------------------------------------------------------//
    bool            AbstractVersionedStoreClient_canJoin                    () const override;
    size_t          AbstractVersionedStoreClient_getJoinScore               () const override;
    size_t          AbstractVersionedStoreClient_getVersionDependency       () const override;
    void            AbstractVersionedStoreClient_joinBranch                 ( VersionedBranch& branch ) override;
    bool            AbstractVersionedStoreClient_preventJoin                () const override;

public:

    //----------------------------------------------------------------//
                    VersionedBranch         ();
                    ~VersionedBranch        ();
};

} // namespace Volition
#endif
