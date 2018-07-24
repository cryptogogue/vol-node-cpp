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
size_t VersionedStoreEpoch::countChildren () const {
    return this->mChildren.size ();
}

//----------------------------------------------------------------//
size_t VersionedStoreEpoch::countClients () const {
    return this->mClients.size ();
}

//----------------------------------------------------------------//
size_t VersionedStoreEpoch::countLayers () const {
    return this->mLayers.size ();
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::copyBackLayerToFront ( VersionedStoreEpoch& epoch ) const {

    const VersionedStoreEpoch& fromEpoch = *this;
    VersionedStoreEpoch& toEpoch = epoch;

    toEpoch.mLayers.insert ( toEpoch.mLayers.begin (), make_unique < Layer >());

    assert ( fromEpoch.mLayers.size () && toEpoch.mLayers.size ());

    const Layer& fromLayer = *fromEpoch.mLayers.back ();
    Layer& toLayer = *toEpoch.mLayers [ 0 ];

    // iterate through all the keys
    set < string >::const_iterator keyIt = fromLayer.cbegin ();
    for ( ; keyIt != fromLayer.cend (); ++keyIt ) {
        const string& key = *keyIt;
        
        // copy the key to the destination layer
        toLayer.insert ( key );
        
        // find the value stack in source epoch
        map < string, unique_ptr < AbstractValueStack >>::const_iterator valueIt = this->mValueStacksByKey.find ( key );
        assert ( valueIt != this->mValueStacksByKey.cend ());
        
        // get the source value stack
        const AbstractValueStack* fromValueStack = valueIt->second.get ();
        assert ( !fromValueStack->isEmpty ());
        
        // affirm the destination value stack
        unique_ptr < AbstractValueStack >& toValueStack = toEpoch.mValueStacksByKey [ key ];
        if ( !toValueStack ) {
            toValueStack = fromValueStack->makeEmptyCopy ();
        }
        
        // push the value to the front of the destination value stack
        toValueStack->pushFrontRaw ( fromValueStack->getRaw ());
    }
}

//----------------------------------------------------------------//
const AbstractValueStack* VersionedStoreEpoch::findValueStack ( string key ) const {

    // descend through epochs until we find the value. if we don't find it, return NULL.
    map < string, unique_ptr < AbstractValueStack >>::const_iterator valueIt = this->mValueStacksByKey.find ( key );
    if ( valueIt != this->mValueStacksByKey.cend ()) {
        return valueIt->second.get ();
    }
    return this->mParent ? this->mParent->findValueStack ( key ) : NULL;
}

//----------------------------------------------------------------//
shared_ptr < VersionedStoreEpoch > VersionedStoreEpoch::getOnlyChild () {

    if ( this->mChildren.size () == 1 ) {
        return ( *this->mChildren.begin ())->shared_from_this ();
    }
    return NULL;
}

//----------------------------------------------------------------//
VersionedStore* VersionedStoreEpoch::getOnlyClient () {

    if ( this->mClients.size () == 1 ) {
        return ( *this->mClients.begin ());
    }
    return NULL;
}

//----------------------------------------------------------------//
shared_ptr < VersionedStoreEpoch > VersionedStoreEpoch::getParent () {

    return this->mParent;
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::moveChildrenTo ( VersionedStoreEpoch& epoch ) {

    set < VersionedStoreEpoch* >::iterator childIt = this->mChildren.begin ();
    while ( childIt != this->mChildren.end()) {
        epoch.mChildren.insert ( *childIt );
        childIt = this->mChildren.erase ( childIt );
    }
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::moveClientTo ( VersionedStore& client, shared_ptr < VersionedStoreEpoch > epoch ) {

    assert ( client.mEpoch.get () == this );
    assert ( this->mClients.find ( &client ) != this->mClients.end ());

    // make sure we're not deleted until done
    shared_ptr < VersionedStoreEpoch > scopedSelf = this->shared_from_this ();

    this->mClients.erase ( &client );
    client.mEpoch = NULL;

    if ( epoch ) {
        epoch->mClients.insert ( &client );
        client.mEpoch = epoch->shared_from_this ();
    }
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::moveClientsTo ( VersionedStoreEpoch& epoch, const VersionedStore* except ) {

    // make sure we're not deleted until done
    shared_ptr < VersionedStoreEpoch > scopedSelf = this->shared_from_this ();

    // move all the other clients to the new epoch
    set < VersionedStore* >::iterator clientIt = this->mClients.begin ();
    while ( clientIt != this->mClients.end ()) {
    
        VersionedStore* client = *clientIt;
        
        if ( client == except ) {
            // client is this, so do nothing and skip to the next client.
            ++clientIt;
        }
        else {
            // point the client at the new epoch and add it to that epoch's client set.
            client->mEpoch = epoch.shared_from_this ();
            epoch.mClients.insert ( client );
            clientIt = this->mClients.erase ( clientIt ); // remove it from the current epoch's set.
        }
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
void VersionedStoreEpoch::setParent ( shared_ptr < VersionedStoreEpoch > parent ) {

    if ( parent != this->mParent ) {
        this->mParent = parent;
        if ( parent ) {
            parent->mChildren.insert ( this );
        }
   }
}

//----------------------------------------------------------------//
VersionedStoreEpoch::VersionedStoreEpoch () {
}

//----------------------------------------------------------------//
VersionedStoreEpoch::~VersionedStoreEpoch () {

    if ( this->mParent ) {
        this->mParent->mChildren.erase ( this );
    }
}

} // namespace Volition
