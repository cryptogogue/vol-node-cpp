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
void VersionedStoreBranch::affirmClient ( AbstractVersionedStoreClient& client ) {

    this->mClients.insert ( &client );
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::countDependencies () const {

    return this->mClients.size ();
}

//----------------------------------------------------------------//
void VersionedStoreBranch::eraseClient ( AbstractVersionedStoreClient& client ) {

    this->mClients.erase ( &client );
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::findImmutableTop ( const AbstractVersionedStoreClient* ignore ) const {

    LOG_SCOPE_F ( INFO, "VersionedStoreBranch::findImmutableTop ()" );

    size_t immutableTop = this->getVersionDependency ();

    set < AbstractVersionedStoreClient* >::const_iterator clientIt = this->mClients.cbegin ();
    for ( ; clientIt != this->mClients.cend (); ++clientIt ) {

        const AbstractVersionedStoreClient* client = *clientIt;
        if ( client != ignore ) {
        
            size_t clientVersion = client->getVersionDependency ();
            
            if ( immutableTop < clientVersion ) {
                immutableTop = clientVersion;
            }
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
    for ( ; branch; version = branch->mBaseVersion, branch = branch->mBranch.get ()) {
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
void VersionedStoreBranch::optimize () {

    LOG_SCOPE_F ( INFO, "VersionedStoreBranch::optimize ()" );
    
    size_t immutableTop = this->mBaseVersion;
    bool preventJoin = this->preventJoin ();
    AbstractVersionedStoreClient* bestJoin = NULL;
    
    LOG_F ( INFO, "evaluating clients for possible concatenation..." );
    set < AbstractVersionedStoreClient* >::const_iterator clientIt = this->mClients.cbegin ();
    for ( ; clientIt != this->mClients.cend (); ++clientIt ) {

        AbstractVersionedStoreClient* client = *clientIt;
        LOG_SCOPE_F ( INFO, "client %p", client );
        
        size_t clientVersion = client->getVersionDependency ();
        if ( immutableTop < clientVersion ) {
            immutableTop = clientVersion;
            bestJoin = NULL;
        }
        
        if (( !preventJoin ) && client->canJoin ()) {

            if ( client->preventJoin ()) {
                preventJoin = true;
                bestJoin = NULL;
            }
            else {
                if (( !bestJoin ) || ( bestJoin->getJoinScore () < client->getJoinScore ())) {
                    LOG_F ( INFO, "found a client that can join" );
                    bestJoin = client;
                }
            }
        }
    }
    
    LOG_F ( INFO, "immutableTop: %d", ( int )immutableTop );
    LOG_F ( INFO, "topVersion: %d", ( int )this->getTopVersion ());
    
    this->truncate ( immutableTop );
    
    if ( bestJoin ) {
        bestJoin->joinBranch ( *this );
    }
}

//----------------------------------------------------------------//
void VersionedStoreBranch::setParent ( shared_ptr < VersionedStoreBranch > parent ) {

    if ( this->mBranch != parent ) {

        if ( this->mBranch ) {
            this->mBranch->eraseClient ( *this );
        }

        this->mBranch = parent;

        if ( parent ) {
            parent->affirmClient ( *this );
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

    this->setParent ( parent->mBaseVersion < baseVersion ? parent : parent->mBranch );
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

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
bool VersionedStoreBranch::AbstractVersionedStoreClient_canJoin () const {
    return true;
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::AbstractVersionedStoreClient_getJoinScore () const {
    return this->getTopVersion ();
}

//----------------------------------------------------------------//
size_t VersionedStoreBranch::AbstractVersionedStoreClient_getVersionDependency () const {
    return this->mBaseVersion;
}

//----------------------------------------------------------------//
void VersionedStoreBranch::AbstractVersionedStoreClient_joinBranch ( VersionedStoreBranch& branch ) {

    LOG_SCOPE_F ( INFO, "VersionedStoreBranch::AbstractVersionedStoreClient_joinBranch ()" );
    LOG_F ( INFO, "JOINING PARENT BRANCH" );
    
    this->optimize ();
    
    shared_ptr < VersionedStoreBranch > pinThis = this->shared_from_this ();
    
    // merge the branch layers
    branch.mLayers.insert ( this->mLayers.begin(), this->mLayers.end ());

    // copy the value stacks
    map < string, unique_ptr < AbstractValueStack >>::iterator valueStackIt = this->mValueStacksByKey.begin ();
    for ( ; valueStackIt != this->mValueStacksByKey.end (); ++valueStackIt ) {
        
        const AbstractValueStack* fromStack = this->findValueStack ( valueStackIt->first );
        assert ( fromStack );
        
        unique_ptr < AbstractValueStack >& toStack = branch.mValueStacksByKey [ valueStackIt->first ];
        if ( !toStack ) {
            toStack = fromStack->makeEmptyCopy ();
        }
        toStack->copyFrom ( *fromStack );
    }

    // copy the clients
    set < AbstractVersionedStoreClient* >::iterator clientIt = this->mClients.begin ();
    for ( ; clientIt != this->mClients.end (); ++clientIt ) {
        AbstractVersionedStoreClient* client = *clientIt;
        branch.affirmClient ( *client );
        client->mBranch = branch.shared_from_this ();
    }
    
    pinThis = NULL;
}

//----------------------------------------------------------------//
bool VersionedStoreBranch::AbstractVersionedStoreClient_preventJoin () const {
    return ( this->mDirectReferenceCount > 0 );
}

} // namespace Volition
