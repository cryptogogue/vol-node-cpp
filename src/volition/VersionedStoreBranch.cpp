// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStoreSnapshot.h>
#include <volition/VersionedStoreBranch.h>

namespace Volition {

//================================================================//
// VersionedStoreBranch
//================================================================//

//----------------------------------------------------------------//
void VersionedStoreBranch::affirmChild ( VersionedStoreBranch& child ) {

    this->mChildren.insert ( &child );
}

//----------------------------------------------------------------//
void VersionedStoreBranch::affirmClient ( VersionedStoreSnapshot& client ) {

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
void VersionedStoreBranch::eraseClient ( VersionedStoreSnapshot& client ) {

    this->mClients.erase ( &client );
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::findImmutableTop ( const VersionedStoreSnapshot* ignore ) const {

    LOG_SCOPE_F ( INFO, "VersionedStoreBranch::findImmutableTop ()" );

    size_t immutableTop = this->getVersionDependency ();

    set < VersionedStoreSnapshot* >::const_iterator clientIt = this->mClients.cbegin ();
    for ( ; clientIt != this->mClients.cend (); ++clientIt ) {

        const VersionedStoreSnapshot* client = *clientIt;
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

    return this->mLayers.size () ? this->mLayers.rbegin ()->first + 1 : 0;
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::getVersionDependency () const {

    return this->mBaseVersion;
}

//----------------------------------------------------------------//
void VersionedStoreBranch::optimize () {

    LOG_SCOPE_F ( INFO, "VersionedStoreBranch::optimize ()" );

    size_t immutableTop = this->findImmutableTop ();
    
    LOG_F ( INFO, "immutableTop: %d", ( int )immutableTop );
    LOG_F ( INFO, "topVersion: %d", ( int )this->getTopVersion ());
    
    this->truncate ( immutableTop );
    
    LOG_F ( INFO, "evaluating children for possible concatenation..." );
    shared_ptr < VersionedStoreBranch > mergeBranch;
    for ( set < VersionedStoreBranch* >::iterator childIt = this->mChildren.begin (); childIt != this->mChildren.end (); ++childIt ) {
        
        VersionedStoreBranch* child = *childIt;
        LOG_SCOPE_F ( INFO, "child %p", child );
        
        if (( child->mDirectReferenceCount == 0 ) && ( child->getVersionDependency () == immutableTop )) {
            
            if (( !mergeBranch ) || ( mergeBranch->getTopVersion () < child->getTopVersion () )) {
                LOG_F ( INFO, "found a mergeable branch" );
                mergeBranch = child->shared_from_this ();
            }
        }
    }
    
    if ( mergeBranch ) {
    
        LOG_F ( INFO, "MERGING CHILD BRANCH" );
    
        weak_ptr < VersionedStoreBranch > weakMergeBranch = mergeBranch;
    
        // merge the branch layers
        this->mLayers.insert ( mergeBranch->mLayers.begin(), mergeBranch->mLayers.end ());
        
        // copy the value stacks
        map < string, unique_ptr < AbstractValueStack >>::iterator valueStackIt = mergeBranch->mValueStacksByKey.begin ();
        for ( ; valueStackIt != mergeBranch->mValueStacksByKey.end (); ++valueStackIt ) {
            
            const AbstractValueStack* fromStack = mergeBranch->findValueStack ( valueStackIt->first );
            assert ( fromStack );
            
            unique_ptr < AbstractValueStack >& toStack = this->mValueStacksByKey [ valueStackIt->first ];
            if ( !toStack ) {
                toStack = fromStack->makeEmptyCopy ();
            }
            toStack->copyFrom ( *fromStack );
        }
        
        // copy the clients
        for ( set < VersionedStoreSnapshot* >::iterator clientIt = mergeBranch->mClients.begin (); clientIt != mergeBranch->mClients.end (); ++clientIt ) {
            VersionedStoreSnapshot* client = *clientIt;
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
    
    Layer& layer = this->mLayers [ version ];
    if ( layer.find ( key ) == layer.end ()) {
        layer.insert ( key );
    }
}

//----------------------------------------------------------------//
void VersionedStoreBranch::truncate ( size_t topVersion ) {

    LOG_SCOPE_F ( INFO, "truncate: %d -> %d", ( int )this->getTopVersion (), ( int )topVersion );

    map < size_t, Layer >::reverse_iterator layerIt = this->mLayers.rbegin ();
    while (( layerIt != this->mLayers.rend ()) && ( layerIt->first >= topVersion )) {
    
        LOG_SCOPE_F ( INFO, "popping layer: %d", ( int )layerIt->first );
        
        Layer& layer = layerIt->second;
        
        Layer::iterator keyIt = layer.begin ();
        for ( ; keyIt != layer.end (); ++keyIt ) {

            unique_ptr < AbstractValueStack >& valueStack = this->mValueStacksByKey [ *keyIt ];
            assert ( valueStack );
            
            valueStack->erase ( layerIt->first );
            if ( valueStack->size () == 0 ) {
                this->mValueStacksByKey.erase ( *keyIt );
            }
        }
        this->mLayers.erase ( layerIt->first );
        layerIt = this->mLayers.rbegin ();
    }
}

//----------------------------------------------------------------//
VersionedStoreBranch::VersionedStoreBranch () :
    mBaseVersion ( 0 ),
    mDirectReferenceCount ( 0 ) {
}

//----------------------------------------------------------------//
VersionedStoreBranch::VersionedStoreBranch ( shared_ptr < VersionedStoreBranch > parent, size_t baseVersion ) :
    mDirectReferenceCount ( 0 ) {
    
    assert ( parent && ( parent->mBaseVersion <= baseVersion ) && ( baseVersion <= parent->getTopVersion ()));

    this->setParent ( parent->mBaseVersion < baseVersion ? parent : parent->mParent );
    this->mBaseVersion = baseVersion;
    
    map < size_t, Layer >::const_iterator layerIt = parent->mLayers.find ( baseVersion );
    if ( layerIt != parent->mLayers.cend ()) {
    
        const Layer& fromLayer = layerIt->second;
        Layer& toLayer = this->mLayers [ baseVersion ];
        
        Layer::const_iterator keyIt = fromLayer.cbegin ();
        for ( ; keyIt != fromLayer.cend (); ++keyIt ) {
            
            toLayer.insert ( *keyIt );
            
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

    assert ( this->mDirectReferenceCount == 0 );
    this->setParent ( NULL );
}

} // namespace Volition
