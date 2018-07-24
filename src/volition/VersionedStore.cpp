// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>

namespace Volition {

//================================================================//
// AbstractValueStack
//================================================================//

//----------------------------------------------------------------//
AbstractValueStack::AbstractValueStack () :
    mTypeID ( typeid ( void ).hash_code ()) {
}

//----------------------------------------------------------------//
AbstractValueStack::~AbstractValueStack () {
}

//----------------------------------------------------------------//
const void* AbstractValueStack::getRaw () const {
    return this->AbstractValueStack_getRaw ();
}

//----------------------------------------------------------------//
bool AbstractValueStack::isEmpty () const {
    return this->AbstractValueStack_isEmpty ();
}

//----------------------------------------------------------------//
unique_ptr < AbstractValueStack > AbstractValueStack::makeEmptyCopy () const {
    return this->AbstractValueStack_makeEmptyCopy ();
}

//----------------------------------------------------------------//
void AbstractValueStack::pop () {
    this->AbstractValueStack_pop ();
}

//----------------------------------------------------------------//
void AbstractValueStack::pushBackRaw ( const void* value ) {
    this->AbstractValueStack_pushBackRaw ( value );
}

//----------------------------------------------------------------//
void AbstractValueStack::pushFrontRaw ( const void* value ) {
    this->AbstractValueStack_pushFrontRaw ( value );
}

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

    toEpoch.mLayers.insert ( toEpoch.mLayers.begin (), make_unique < VersionedStoreLayer >());

    assert ( fromEpoch.mLayers.size () && toEpoch.mLayers.size ());

    const VersionedStoreLayer& fromLayer = *fromEpoch.mLayers.back ();
    VersionedStoreLayer& toLayer = *toEpoch.mLayers [ 0 ];

    // iterate through all the keys
    set < string >::const_iterator keyIt = fromLayer.mKeys.cbegin ();
    for ( ; keyIt != fromLayer.mKeys.cend (); ++keyIt ) {
        const string& key = *keyIt;
        
        // copy the key to the destination layer
        toLayer.mKeys.insert ( key );
        
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

        const VersionedStoreLayer* layer = this->mLayers.back ().get ();
        set < string >::const_iterator keyIt = layer->mKeys.cbegin ();
        for ( ; keyIt != layer->mKeys.cend (); ++keyIt ) {
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

    this->mLayers.push_back ( make_unique < VersionedStoreLayer >());
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

//================================================================//
// VersionedStore
//================================================================//

//----------------------------------------------------------------//
void VersionedStore::affirmEpoch () {

    if ( !this->mEpoch ) {
        this->mEpoch = make_shared < VersionedStoreEpoch >();
        this->mEpoch->mClients.insert ( this );
        this->mEpoch->pushLayer ();
    }
}

//----------------------------------------------------------------//
void VersionedStore::clear () {

    if ( this->mEpoch ) {
        this->mEpoch->mClients.erase ( this );
        this->mEpoch = NULL;
    }
}

//----------------------------------------------------------------//
const void* VersionedStore::getRaw ( string key, size_t typeID ) const {

    assert ( this->mEpoch );
    const AbstractValueStack* valueStack = this->mEpoch->findValueStack ( key );
    
    if ( valueStack ) {
        assert ( valueStack->mTypeID == typeID );
        return valueStack->getRaw ();
    }
    return NULL;
}

//----------------------------------------------------------------//
void VersionedStore::popVersion () {

    // client C popping layer in epoch E, with children == 0 && clients == 1
    //      if E has layers == 1
    //          assign C to E-1
    //      if E has layers > 1
    //          nothing special to do; just pop the version
    //
    // client C popping layer in epoch E, with children > 0 || clients > 1
    //
    //      if E has layers == 1
    //
    //          if E has children == 1 && clients == 1
    //              assign C to E-1
    //              set E+1 as child of E-1
    //
    //          else: E has children > 1 || clients > 1
    //              assign C to E-1
    //
    //      if E has layers > 1
    //
    //          if E has children == 1 && clients == 1
    //              copy the top layer of E to become the base of E+1
    //              pop the top layer of E
    //              assign C to E-1
    //
    //          else: E has children > 1 || other clients > 1
    //              copy the top layer of E into a new epoch A
    //              pop the top layer of E
    //              assign all other clients and children to A
    //              set A as a child of E

    assert ( this->mEpoch );
    
    size_t nChildren    = this->mEpoch->countChildren ();
    size_t nClients     = this->mEpoch->countClients ();
    size_t nLayers      = this->mEpoch->countLayers ();

    assert ( nClients > 0 );
    assert ( nLayers > 0 );
    
    assert (( nLayers > 1 ) || ( this->mEpoch->mParent ));
    
    // nothing special to do; just pop the version

    if (( nChildren == 0 ) && ( nClients == 1  )) {

        // client C popping layer in epoch E, with children == 0 && clients == 1
        
        if ( nLayers == 1 ) {
        
            // E has layers == 1
            
            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochEParent     = epochE->getParent ();

            // assign C to E-1
            epochE->moveClientTo ( *this, epochEParent );
        }
        else {
        
            // E has layers > 1
            assert ( nLayers > 1 );

            this->mEpoch->popLayer ();
        }
        return;
    }
    
    // client C popping layer in epoch E, with children > 0 || other clients > 1
    assert (( nChildren > 0 ) || ( nClients > 1 ));
    
    if ( nLayers == 1 ) {
        
        // E has layers == 1
    
        if (( nChildren == 1 ) && ( nClients == 1 )) {
        
            // E has children == 1 && clients == 1
            
            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochEParent     = epochE->getParent ();
            shared_ptr < VersionedStoreEpoch > epochEChild      = epochE->getOnlyChild ();
            
            // assign C to E-1
            epochE->moveClientTo ( *this, epochEParent );
            
            // set E+1 as child of E-1
            epochEChild->setParent ( epochEParent );
            
            // E goes away (no client or children to keep it around)
        }
        else {
        
            // E has children > 1 || clients > 1
            assert (( nChildren > 1 ) || ( nClients > 1 ));
        
            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochEParent     = epochE->getParent ();
        
            // assign C to E-1
            epochE->moveClientTo ( *this, epochEParent );
        }
    }
    else {
        
        // if E has layers > 1
        assert ( nLayers > 1 );
        
        if (( nChildren == 1 ) && ( nClients == 1 )) {
        
            // E has children == 1 && clients == 1
            
            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochEParent     = epochE->getParent ();
            shared_ptr < VersionedStoreEpoch > epochEChild      = epochE->getOnlyChild ();
            
            // copy the top layer of E to become the base of E+1
            epochE->copyBackLayerToFront ( *epochEChild );
            
            // pop the top layer of E
            epochE->popLayer ();
            
            // assign C to E-1
            epochE->moveClientTo ( *this, epochEParent );
        }
        else {
        
            // E has children > 1 || other clients > 1
            assert (( nChildren > 1 ) || ( nClients > 1 ));
            
            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochEParent     = epochE->getParent ();
            shared_ptr < VersionedStoreEpoch > epochEChild      = epochE->getOnlyChild ();
            shared_ptr < VersionedStoreEpoch > epochA           = make_shared < VersionedStoreEpoch >();
            
            // copy the top layer of E into a new epoch A
            epochE->copyBackLayerToFront ( *epochA );
            
            // pop the top layer of E
            epochE->popLayer ();
            
            // assign all other clients and children to A
            epochE->moveClientsTo ( *epochA );
            epochE->moveChildrenTo ( *epochA );
            
            // set A as a child of E
            epochA->setParent ( epochE );
        }
    }
    
    this->affirmEpoch ();
}

//----------------------------------------------------------------//
void VersionedStore::prepareForSetValue () {
    
    // client C writing to epoch E, with children == 0 && clients == 1
    //      nothing special to do; just set the value
    //
    // client C writing to epoch E, with children > 0 || clients > 1
    //
    //      if E has layers == 1
    //
    //          if E has children == 1 && clients == 1
    //              copy the top layer of E into a new epoch A
    //              copy the top layer of E to become the base of E+1
    //              assign C to A
    //              set A, E+1 as children of E-1
    //
    //          else: E has children > 1 || clients > 1
    //              copy the top layer of E into a new epoch A
    //              assign C to A
    //              assign all other clients and children to E
    //              set A as child of E-1
    //
    //      if E has layers > 1
    //
    //          if E has children == 1 && clients == 1
    //              copy the top layer of E into a new epoch A
    //              copy the top layer of E to become the base of E+1
    //              pop the top layer of E
    //              assign C to A
    //              set A as a child of E
    //
    //          else: E has children > 1 || other clients > 1
    //              copy the top layer of E into new epochs A, B
    //              pop the top layer of E
    //              assign C to A
    //              assign all other clients and children to B
    //              set A, B as children of E

    assert ( this->mEpoch );
    
    size_t nChildren    = this->mEpoch->countChildren ();
    size_t nClients     = this->mEpoch->countClients ();
    size_t nLayers      = this->mEpoch->countLayers ();

    assert ( nClients > 0 );
    assert ( nLayers > 0 );

    if (( nChildren == 0 ) && ( nClients == 1 )) {
        // client C writing to epoch E, with children == 0 && clients == 1
        // nothing special to do; just set the value
        return;
    }
    
    // client C writing to epoch E, with children > 0 || other clients > 1
    assert (( nChildren > 0 ) || ( nClients > 1 ));
    
    if ( nLayers == 1 ) {
        
        // E has layers == 1
    
        if (( nChildren == 1 ) && ( nClients == 1 )) {
        
            // E has children == 1 && clients == 1

            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochEParent     = epochE->getParent ();
            shared_ptr < VersionedStoreEpoch > epochEChild      = epochE->getOnlyChild ();
            shared_ptr < VersionedStoreEpoch > epochA           = make_shared < VersionedStoreEpoch >();
            
            // copy the top layer of E into epoch A
            epochE->copyBackLayerToFront ( *epochA );
            
            // copy the top layer of E to become the base of E+1
            epochE->copyBackLayerToFront ( *epochEChild );
            
            // assign C to A
            epochE->moveClientTo ( *this, epochA );
            
            // set A, E+1 as children of E-1
            epochA->setParent ( epochEParent );
            epochEChild->setParent ( epochEParent );
            
            // E goes away (no client or children to keep it around)
        }
        else {
        
            // E has children > 1 || clients > 1
            assert (( nChildren > 1 ) || ( nClients > 1 ));
            
            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochEParent     = epochE->getParent ();
            shared_ptr < VersionedStoreEpoch > epochA           = make_shared < VersionedStoreEpoch >();
            
            // copy the top layer of E into epoch A
            epochE->copyBackLayerToFront ( *epochA );
            
            // assign C to A
            epochE->moveClientTo ( *this, epochA );
            
            // set A as child of E-1
            epochA->setParent ( epochEParent );
        }
    }
    else {
        
        // E has layers > 1
        assert ( nLayers > 1 );
        
        if (( nChildren == 1 ) && ( nClients == 1 )) {
        
            // E has children == 1 && clients == 1
            
            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochEChild      = epochE->getOnlyChild ();
            shared_ptr < VersionedStoreEpoch > epochA           = make_shared < VersionedStoreEpoch >();
            
            // copy the top layer of E into epoch A
            epochE->copyBackLayerToFront ( *epochA );
            
            // copy the top layer of E to become the base of E+1
            epochE->copyBackLayerToFront ( *epochEChild );
            
            // pop the top layer of E
            epochE->popLayer ();
            
            // assign C to A
            epochE->moveClientTo ( *this, epochA );
            
            // set A as a child of E
            epochA->setParent ( epochE );
        }
        else {
        
            // E has children > 1 || other clients > 1
            assert (( nChildren > 1 ) || ( nClients > 1 ));
            
            shared_ptr < VersionedStoreEpoch > epochE           = this->mEpoch;
            shared_ptr < VersionedStoreEpoch > epochA           = make_shared < VersionedStoreEpoch >();
            shared_ptr < VersionedStoreEpoch > epochB           = make_shared < VersionedStoreEpoch >();
            
            // copy the top layer of E into epoch A
            epochE->copyBackLayerToFront ( *epochA );
            
            // copy the top layer of E into epoch B
            epochE->copyBackLayerToFront ( *epochB );
            
            // pop the top layer of E
            epochE->popLayer ();
            
            // assign C to A
            epochE->moveClientTo ( *this, epochA );
            
            // assign all other clients and children to B
            epochE->moveClientsTo ( *epochB );
            epochE->moveChildrenTo ( *epochB );
            
            // set A, B as children of E
            epochA->setParent ( epochE );
            epochB->setParent ( epochE );
        }
    }
}

//----------------------------------------------------------------//
void VersionedStore::pushVersion () {

    assert ( this->mEpoch );
    
    // if more clients, can't change this epoch - gotta make a new one
    if (( this->mEpoch->countClients () > 1 ) || ( this->mEpoch->countChildren () > 0 )) {
        
        // nice and easy - don't have to modify any other clients. just make and set the new epoch.
        shared_ptr < VersionedStoreEpoch > epoch = make_shared < VersionedStoreEpoch >();
        epoch->setParent ( this->mEpoch );
        this->mEpoch->moveClientTo ( *this, epoch );
    }
    
    // make the new layer
    this->mEpoch->pushLayer ();
}

//----------------------------------------------------------------//
void VersionedStore::setRaw ( string key, size_t typeID, const void* value ) {

    assert ( this->mEpoch );
    assert ( this->mEpoch->mLayers.size () > 0 );

    this->mEpoch->mLayers.back ()->mKeys.insert ( key );

    AbstractValueStack* valueStack = this->mEpoch->mValueStacksByKey [ key ].get ();
    assert ( valueStack );

    valueStack->pushBackRaw ( value );
}

//----------------------------------------------------------------//
void VersionedStore::takeSnapshot ( VersionedStore& other ) {

    this->mEpoch->moveClientTo ( *this, other.mEpoch );
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore () {

    this->affirmEpoch ();
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

    this->mEpoch->moveClientTo ( *this, NULL );
}

} // namespace Volition
