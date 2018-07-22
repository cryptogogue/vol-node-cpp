// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>

namespace Volition {

//================================================================//
// AbstractVersionedValue
//================================================================//

//----------------------------------------------------------------//
AbstractVersionedValue::~AbstractVersionedValue () {
}

//----------------------------------------------------------------//
unique_ptr < AbstractVersionedValue > AbstractVersionedValue::copyTop () const {
    return this->AbstractVersionedValue_copyTop ();
}

//----------------------------------------------------------------//
void* AbstractVersionedValue::getRaw () const {
    return this->AbstractVersionedValue_getRaw ();
}

//----------------------------------------------------------------//
bool AbstractVersionedValue::isEmpty () const {
    return this->AbstractVersionedValue_isEmpty ();
}

//----------------------------------------------------------------//
void AbstractVersionedValue::pop () {
    this->AbstractVersionedValue_pop ();
}

//----------------------------------------------------------------//
void AbstractVersionedValue::setRaw ( const void* value ) {
    this->AbstractVersionedValue_setRaw ( value );
}

//================================================================//
// VersionedStoreEpoch
//================================================================//

//----------------------------------------------------------------//
const void* VersionedStoreEpoch::getRaw ( string key ) const {

    // descend through epochs until we find the value. if we don't find it,
    // return NULL.
    map < string, unique_ptr < AbstractVersionedValue >>::const_iterator valueIt = this->mMap.find ( key );
    if ( valueIt != this->mMap.cend ()) {
        assert ( valueIt->second );
        return valueIt->second->getRaw ();
    }
    return this->mParent ? this->mParent->getRaw ( key ) : NULL;
}

//----------------------------------------------------------------//
void VersionedStoreEpoch::setRaw ( string key, const void* value ) {

    AbstractVersionedValue* versionedValue = this->mMap [ key ].get ();
    assert ( versionedValue );
    versionedValue->setRaw ( value );
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

//================================================================//
// VersionedStore
//================================================================//

//----------------------------------------------------------------//
void VersionedStore::popVersion () {

    assert ( this->mEpoch );

    // if more clients, can't change this epoch - gotta make a new one
    if ( this->mEpoch->mClients.size () > 1 ) {
    
        // unlike pushVersion (), we have to make the new epoch then update all the *other*
        // clients to point to it. we also have to re-root any values held in the current
        // epoch's top layer.
    
        // make a new epoch to hold all the other client's we're leaving behind
        shared_ptr < VersionedStoreEpoch > epoch = make_shared < VersionedStoreEpoch >();
        epoch->mParent = this->mEpoch; // our epoch will not change
        
        // move all the other clients to the new epoch
        set < VersionedStore* >::iterator clientIt = this->mEpoch->mClients.begin ();
        while ( clientIt != this->mEpoch->mClients.end ()) {
        
            VersionedStore* client = *clientIt;
            
            if ( client == this ) {
                // client is this, so do nothing and skip to the next client.
                ++clientIt;
            }
            else {
                // point the client at the new epoch and add it to that epoch's client set.
                client->mEpoch = epoch;
                epoch->mClients.insert ( client );
                clientIt = this->mEpoch->mClients.erase ( clientIt ); // remove it from the current epoch's set.
            }
        }
        
        // and now the fun begins. since we're going to remove a layer, but want the new epoch (representing the
        // original state of the store) to keep any values specified in this layer, we have to move any values
        // named in our current layer in the base layer of the new epoch.
        
        // here's the new layer
        epoch->mLayers.push_back ( make_unique < VersionedStoreLayer >());
        VersionedStoreLayer* newLayer = epoch->mLayers.back ().get ();
        const VersionedStoreLayer* layer = this->mEpoch->mLayers.back ().get ();
        
        set < string >::const_iterator keyIt = layer->mKeys.cbegin ();
        for ( ; keyIt != layer->mKeys.cend (); ++keyIt ) {
            const string& key = *keyIt;
            
            const AbstractVersionedValue* value = this->mEpoch->mMap [ key ].get ();
            assert ( value );
            
            epoch->mMap [ key ] = value->copyTop ();
            newLayer->mKeys.insert ( key );
        }
    }
    
    // before we pop the layer, we need to pop any values it contains. we'll also need to erase any values
    // that no longer have version history.
    const VersionedStoreLayer* layer = this->mEpoch->mLayers.back ().get ();
    set < string >::const_iterator keyIt = layer->mKeys.cbegin ();
    for ( ; keyIt != layer->mKeys.cend (); ++keyIt ) {
        const string& key = *keyIt;
        
        AbstractVersionedValue* value = this->mEpoch->mMap [ key ].get ();
        assert ( value );
        
        value->pop (); // pop the value.
        
        // if it's empty now (no more versions), remove it.
        if ( value->isEmpty ()) {
            this->mEpoch->mMap.erase ( key );
        }
    }
    this->mEpoch->mLayers.pop_back ();
}

//----------------------------------------------------------------//
void VersionedStore::pushVersion () {

    // create the epoch if it doesn't exist
    if ( !this->mEpoch ) {
        this->mEpoch = make_shared < VersionedStoreEpoch >();
        this->mEpoch->mClients.insert ( this );
    }
    
    // if more clients, can't change this epoch - gotta make a new one
    if ( this->mEpoch->mClients.size () > 1 ) {
        
        // nice and easy - don't have to modify any other clients. just make and set the new epoch.
        shared_ptr < VersionedStoreEpoch > epoch = make_shared < VersionedStoreEpoch >();
        epoch->mParent = this->mEpoch;
        epoch->mClients.insert ( this );
        
        this->mEpoch->mClients.erase ( this );
        this->mEpoch = epoch;
    }
    
    // make the new layer
    this->mEpoch->mLayers.push_back ( make_unique < VersionedStoreLayer >());
}

//----------------------------------------------------------------//
void VersionedStore::takeSnapshot ( VersionedStore& other ) {

    if ( other.mEpoch ) {
        this->mEpoch = other.mEpoch;
        other.mEpoch->mClients.insert ( this );
    }
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore () {
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore ( VersionedStore& other ) {

    this->takeSnapshot ( other );
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore ( const VersionedStore& other ) {
    assert ( false );
}

//----------------------------------------------------------------//
VersionedStore::~VersionedStore () {

    if ( this->mEpoch ) {
        this->mEpoch->mClients.erase ( this );
        this->mEpoch = NULL;
    }
}

} // namespace Volition
