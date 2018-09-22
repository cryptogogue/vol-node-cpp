// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>
#include <volition/VersionedStoreBranch.h>

//#define DEBUG_LOG printf ( "%04x:  ", ( int )(( size_t )this ) & 0xffff ); printf
#define DEBUG_LOG(...)

namespace Volition {

//================================================================//
// VersionedStoreBranch
//================================================================//

//----------------------------------------------------------------//
void VersionedStoreBranch::affirmChild ( VersionedStoreBranch& child ) {

    this->mChildren.insert ( &child );
}

//----------------------------------------------------------------//
void VersionedStoreBranch::affirmClient ( VersionedStore& client ) {

    this->mClients.insert ( &client );
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::countDependencies () const {

    return ( this->mClients.size () + this->mChildren.size ());
}

//----------------------------------------------------------------//
void VersionedStoreBranch::eraseChild ( VersionedStoreBranch& child ) {

    this->mChildren.erase ( &child );
}

//----------------------------------------------------------------//
void VersionedStoreBranch::eraseClient ( VersionedStore& client ) {

    this->mClients.erase ( &client );
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::findImmutableTop ( const VersionedStore* ignore ) const {

    LOG_SCOPE_F ( INFO, "VersionedStoreBranch::findImmutableTop ()" );

    size_t immutableTop = this->getVersionDependency ();

    set < VersionedStore* >::const_iterator clientIt = this->mClients.cbegin ();
    for ( ; clientIt != this->mClients.cend (); ++clientIt ) {

        const VersionedStore* client = *clientIt;
        if ( client != ignore ) {
        
            size_t clientVersion = client->getVersionDependency ();
            
            if ( clientVersion > immutableTop ) {
                immutableTop = clientVersion;
            }
        }
    }
    
    set < VersionedStoreBranch* >::const_iterator childIt = this->mChildren.cbegin ();
    for ( ; childIt != this->mChildren.cend (); ++childIt ) {

        const VersionedStoreBranch* child = *childIt;
        
        size_t clientVersion = child->getVersionDependency ();
            
        if ( clientVersion > immutableTop ) {
            immutableTop = clientVersion;
        }
    }
    
    LOG_F ( INFO, "immutableTop: %d", ( int )immutableTop );
    
    return immutableTop;
}

//----------------------------------------------------------------//
const AbstractValueStack* VersionedStoreBranch::findValueStack ( string key ) const {

    map < string, unique_ptr < AbstractValueStack >>::const_iterator valueIt = this->mValueStacksByKey.find ( key );
    return ( valueIt != this->mValueStacksByKey.cend ()) ? valueIt->second.get () : NULL;
}

//----------------------------------------------------------------//
const void* VersionedStoreBranch::getRaw ( size_t version, string key, size_t typeID ) const {

    const VersionedStoreBranch* branch = this;
    for ( ; branch; version = branch->mBaseVersion, branch = branch->mParent.get ()) {
        if ( branch->mBaseVersion <= version ) {
        
            const AbstractValueStack* valueStack = branch->findValueStack ( key );
            
            if ( valueStack ) {
                assert ( valueStack->mTypeID == typeID );
                const void* value = valueStack->getRaw ( version );
                if ( value ) {
                    return value;
                }
            }
        }
    }
    return NULL;
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::getTopVersion () const {

    return this->mBranchLayers.size () ? this->mBranchLayers.rbegin ()->first + 1 : 0;
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::getVersionDependency () const {

    return this->mBaseVersion;
}

//----------------------------------------------------------------//
void VersionedStoreBranch::optimize () {

    LOG_SCOPE_F ( INFO, "VersionedStoreBranch::optimize ()" );

    VersionedStore* topClient = NULL;
    VersionedStoreBranch* topChild = NULL;

    LOG_SCOPE_F ( INFO, "evaluating clients..." );
    for ( set < VersionedStore* >::iterator clientIt = this->mClients.begin (); clientIt != this->mClients.end (); ++clientIt ) {

        VersionedStore* client = *clientIt;
        LOG_SCOPE_F ( INFO, "client %p", client );
        
        if (( topClient == NULL ) || ( topClient->getVersionDependency () < client->getVersionDependency ())) {
            topClient = client;
            LOG_F ( INFO, "topClient: %04x version: %d", ( int )(( size_t )client ) & 0xffff, ( int ) client->getVersionDependency ());
        }
    }
    
    LOG_SCOPE_F ( INFO, "evaluating children..." );
    for ( set < VersionedStoreBranch* >::iterator childIt = this->mChildren.begin (); childIt != this->mChildren.end (); ++childIt ) {
        
        VersionedStoreBranch* child = *childIt;
        LOG_SCOPE_F ( INFO, "child %p", child );
        
        bool replace = ( topChild == NULL ) || ( topChild->getVersionDependency () < child->getVersionDependency ());
        replace = replace || (( topChild->getVersionDependency () == child->getVersionDependency ()) && ( topChild->getTopVersion () < child->getTopVersion ()));
        
        if ( replace ) {
            topChild = child;
            LOG_F ( INFO, "topClient: %04x topChild: %d", ( int )(( size_t )child ) & 0xffff, ( int ) child->getVersionDependency ());
        }
    }

    size_t immutableTop = topClient ? ( topClient->getVersionDependency ()) : 0;
    if ( topChild && ( immutableTop < topChild->getVersionDependency ())) {
        immutableTop = topChild->getVersionDependency ();
    }
    
    LOG_F ( INFO, "immutableTop: %d", ( int )immutableTop );
    LOG_F ( INFO, "topVersion: %d", ( int )this->getTopVersion ());
    
    if ( immutableTop < this->getTopVersion ()) {
        
        size_t topVersion = this->getTopVersion ();
        for ( size_t i = immutableTop; i < topVersion; ++i ) {
            this->popLayer ();
        }
        assert ( this->getTopVersion () == immutableTop );
    }
    
    if ( topChild && (( topClient == NULL ) || (( topClient->getVersionDependency ()) <= topChild->getVersionDependency ()))) {
    
        LOG_F ( INFO, "MERGING CHILD EPOCH" );
    
        shared_ptr < VersionedStoreBranch > mergeBranch = topChild->shared_from_this ();
        weak_ptr < VersionedStoreBranch > weakMergeBranch = mergeBranch;
    
        // merge the branch layers
        this->mBranchLayers.insert ( mergeBranch->mBranchLayers.begin(), mergeBranch->mBranchLayers.end ());
        
        // copy the value stacks
        map < string, unique_ptr < AbstractValueStack >>::iterator valueStackIt = mergeBranch->mValueStacksByKey.begin ();
        for ( ; valueStackIt != topChild->mValueStacksByKey.end (); ++valueStackIt ) {
            
            const AbstractValueStack* fromStack = mergeBranch->findValueStack ( valueStackIt->first );
            assert ( fromStack );
            
            unique_ptr < AbstractValueStack >& toStack = this->mValueStacksByKey [ valueStackIt->first ];
            if ( !toStack ) {
                toStack = fromStack->makeEmptyCopy ();
            }
            toStack->copyFrom ( *fromStack );
        }
        
        // copy the clients
        for ( set < VersionedStore* >::iterator clientIt = mergeBranch->mClients.begin (); clientIt != mergeBranch->mClients.end (); ++clientIt ) {
            VersionedStore* client = *clientIt;
            this->affirmClient ( *client );
            client->mBranch = this->shared_from_this ();
        }
        
        // copy the children
        for ( set < VersionedStoreBranch* >::iterator childIt = mergeBranch->mChildren.begin (); childIt != mergeBranch->mChildren.end (); ++childIt ) {
            VersionedStoreBranch* child = *childIt;
            this->affirmChild ( *child );
            child->mParent = this->shared_from_this ();
        }

        mergeBranch = NULL;
        assert ( weakMergeBranch.expired ());
        
        this->optimize ();
    }
}

//----------------------------------------------------------------//
void VersionedStoreBranch::popLayer () {

    LOG_SCOPE_F ( INFO, "VersionedStoreBranch::popLayer ()" );

    map < size_t, BranchLayer >::reverse_iterator layerIt = this->mBranchLayers.rbegin ();
    if ( layerIt != this->mBranchLayers.rend ()) {
    
        LOG_SCOPE_F ( INFO, "popping layer: %d", ( int )layerIt->first );
        
        BranchLayer& layer = layerIt->second;
        
        BranchLayer::iterator keyIt = layer.begin ();
        for ( ; keyIt != layer.end (); ++keyIt ) {

            unique_ptr < AbstractValueStack >& valueStack = this->mValueStacksByKey [ *keyIt ];
            assert ( valueStack );
            
            valueStack->erase ( layerIt->first );
            if ( valueStack->size () == 0 ) {
                this->mValueStacksByKey.erase ( *keyIt );
            }
        }
        
        this->mBranchLayers.erase ( layerIt->first );
    }
}

//----------------------------------------------------------------//
void VersionedStoreBranch::setParent ( shared_ptr < VersionedStoreBranch > parent ) {

    if ( this->mParent != parent ) {

        if ( this->mParent ) {
            this->mParent->eraseChild ( *this );
        }

        this->mParent = parent;

        if ( parent ) {
            parent->affirmChild ( *this );
        }
    }
}

//----------------------------------------------------------------//
void VersionedStoreBranch::setRaw ( size_t version, string key, const void* value ) {

    this->mValueStacksByKey [ key ]->setRaw ( version, value );
    
    BranchLayer& layer = this->mBranchLayers [ version ];
    if ( layer.find ( key ) == layer.end ()) {
        layer.insert ( key );
    }
}

//----------------------------------------------------------------//
VersionedStoreBranch::VersionedStoreBranch () :
    mBaseVersion ( 0 ) {
}

//----------------------------------------------------------------//
VersionedStoreBranch::VersionedStoreBranch ( shared_ptr < VersionedStoreBranch > parent, size_t baseVersion ) {
    
    assert ( parent && ( parent->mBaseVersion <= baseVersion ) && ( baseVersion <= parent->getTopVersion ()));

    this->setParent ( parent->mBaseVersion < baseVersion ? parent : parent->mParent );
    this->mBaseVersion = baseVersion;
    
    map < size_t, BranchLayer >::const_iterator layerIt = parent->mBranchLayers.find ( baseVersion );
    if ( layerIt != parent->mBranchLayers.cend ()) {
    
        const BranchLayer& fromLayer = layerIt->second;
        
        BranchLayer::const_iterator keyIt = fromLayer.cbegin ();
        for ( ; keyIt != fromLayer.cend (); ++keyIt ) {
            
            const AbstractValueStack* fromStack = parent->findValueStack ( *keyIt );
            assert ( fromStack );
            
            unique_ptr < AbstractValueStack >& toStack = this->mValueStacksByKey [ *keyIt ];
            if ( !toStack ) {
                toStack = fromStack->makeEmptyCopy ();
            }
            toStack->setRaw ( baseVersion, fromStack->getRaw ( baseVersion ));
        }
    }
}

//----------------------------------------------------------------//
VersionedStoreBranch::~VersionedStoreBranch () {

    this->setParent ( NULL );
}

} // namespace Volition
