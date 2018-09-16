// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>
#include <volition/VersionedStoreEpoch.h>

namespace Volition {

//================================================================//
// VersionedStoreEpoch
//================================================================//

//----------------------------------------------------------------//
void VersionedStoreEpoch::affirmClient ( AbstractVersionedStoreEpochClient& client ) {

    this->mClients.insert ( &client );
}

//----------------------------------------------------------------//
bool VersionedStoreEpoch::containsVersion ( size_t version ) const {

    return (( this->mBaseVersion <= version ) && ( version < ( this->mBaseVersion + this->mLayers.size ())));
}

//----------------------------------------------------------------//
size_t VersionedStoreEpoch::countClients () const {
    return this->mClients.size ();
}

//----------------------------------------------------------------//
VersionedStoreDownstream VersionedStoreEpoch::countDownstream ( size_t version ) const {

    VersionedStoreDownstream downstream;
    
    downstream.mPeers = 0;
    downstream.mDependents = 0;
    
    set < AbstractVersionedStoreEpochClient* >::iterator clientIt = this->mClients.begin ();
    for ( ; clientIt != this->mClients.end (); ++clientIt ) {
    
        AbstractVersionedStoreEpochClient* client = *clientIt;
        size_t clientVersion = client->getVersion ();
        
        if ( clientVersion > version ) {
            downstream.mDependents++;
        }
        else if ( clientVersion == version ) {
            downstream.mPeers++;
        }
    }
    
    downstream.mTotal = downstream.mPeers + downstream.mDependents;
    
    return downstream;
}

//----------------------------------------------------------------//
size_t VersionedStoreEpoch::countLayers () const {
    return this->mLayers.size ();
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::eraseClient ( AbstractVersionedStoreEpochClient& client ) {

    this->mClients.erase ( &client );
}

//----------------------------------------------------------------//
const AbstractValueStack* VersionedStoreEpoch::findValueStack ( string key ) const {

    map < string, unique_ptr < AbstractValueStack >>::const_iterator valueIt = this->mValueStacksByKey.find ( key );
    return ( valueIt != this->mValueStacksByKey.cend ()) ? valueIt->second.get () : NULL;
}

//----------------------------------------------------------------//
const AbstractValueStack* VersionedStoreEpoch::findValueStack ( string key, size_t version ) const {

    assert ( version < ( this->mBaseVersion + this->mLayers.size ()));

    size_t sub = 0;
    if ( version >= this->mBaseVersion ) {

        // see if we have the value in the current epoch.
        const AbstractValueStack* valueStack = this->findValueStack ( key );
        if ( valueStack ) {
            return valueStack;
        }
        sub = 1;
    }
    return this->mParent ? this->mParent->findValueStack ( key, version - sub ) : NULL;
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::optimize () {

    // TODO: implement consolidation of linear runs
    
    if ( this->mClients.size () == 0 ) return;

    size_t maxVersion = 0;
    
    set < AbstractVersionedStoreEpochClient* >::iterator clientIt = this->mClients.begin ();
    for ( ; clientIt != this->mClients.end (); ++clientIt ) {
    
        AbstractVersionedStoreEpochClient* client = *clientIt;
        size_t clientVersion = client->getVersion ();
        
        if ( clientVersion > maxVersion ) {
            maxVersion = clientVersion;
        }
    }
    
    size_t top = maxVersion + 1;
    if ( this->mLayers.size () > top ) {
        this->mLayers.resize ( top );
    }
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::popLayer () {

    // if we're not popping the bottom layer, we need to pop any values it contains.
    // we'll also need to erase any values that no longer have version history.
    if ( this->mLayers.size () > 1 ) {

        const Layer& layer = *this->mLayers.back ();
        set < string >::const_iterator keyIt = layer.cbegin ();
        for ( ; keyIt != layer.cend (); ++keyIt ) {
            const string& key = *keyIt;

            AbstractValueStack* valueStack = this->mValueStacksByKey [ key ].get ();
            assert ( valueStack );

            valueStack->pop (); // pop the value.

            // if it's empty now (no more versions), remove it.
            if ( valueStack->isEmpty ()) {
                this->mValueStacksByKey.erase ( key );
            }
        }
    }
    this->mLayers.pop_back ();
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::pushLayer () {

    this->mLayers.push_back ( make_unique < Layer >());
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::setParent ( shared_ptr < VersionedStoreEpoch > parent, size_t baseVersion ) {

    if ( this->mParent != parent ) {
        
        if ( this->mParent ) {
            this->mParent->eraseClient ( *this );
        }
        
        this->mParent = parent;
        
        if ( parent ) {
            parent->affirmClient ( *this );
        }
    }
    // TODO: add sanity check
    this->mBaseVersion = baseVersion;
}

//----------------------------------------------------------------//
VersionedStoreEpoch::VersionedStoreEpoch () :
    mBaseVersion ( 0 ) {

    this->pushLayer ();
}

//----------------------------------------------------------------//
VersionedStoreEpoch::VersionedStoreEpoch ( shared_ptr < VersionedStoreEpoch > parent, size_t version ) {

    assert ( parent );

    this->setParent ( parent, version );
    this->pushLayer ();

    size_t layerID = version - parent->mBaseVersion;
    
    assert ( layerID < parent->mLayers.size ());

    const Layer& fromLayer = *parent->mLayers [ layerID ];
    Layer& toLayer = *this->mLayers [ 0 ];

    // iterate through all the keys
    set < string >::const_iterator keyIt = fromLayer.cbegin ();
    for ( ; keyIt != fromLayer.cend (); ++keyIt ) {
        const string& key = *keyIt;
        
        // copy the key to the destination layer
        toLayer.insert ( key );
        
        // find the value stack in source epoch
        map < string, unique_ptr < AbstractValueStack >>::const_iterator valueIt = parent->mValueStacksByKey.find ( key );
        assert ( valueIt != parent->mValueStacksByKey.cend ());
        
        // get the source value stack
        const AbstractValueStack* fromValueStack = valueIt->second.get ();
        assert ( !fromValueStack->isEmpty ());
        
        // affirm the destination value stack
        unique_ptr < AbstractValueStack >& toValueStack = this->mValueStacksByKey [ key ];
        toValueStack = fromValueStack->makeEmptyCopy ();
        
        // push the value to the destination value stack
        toValueStack->pushBackRaw ( fromValueStack->getRaw ( version ), version );
    }
}

//----------------------------------------------------------------//
VersionedStoreEpoch::~VersionedStoreEpoch () {

    this->setParent ( NULL, 0 );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
size_t VersionedStoreEpoch::AbstractVersionedStoreEpochClient_getVersion () const {

    return this->mBaseVersion;
}

} // namespace Volition
