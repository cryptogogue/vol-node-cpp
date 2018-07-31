// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>

namespace Volition {

//================================================================//
// VersionedStore
//================================================================//

//----------------------------------------------------------------//
void VersionedStore::affirmEpoch () {

    if ( !this->mEpoch ) {
        this->setEpoch ( make_shared < VersionedStoreEpoch >());
    }
}

//----------------------------------------------------------------//
void VersionedStore::clear () {

    this->setEpoch ( NULL );
    this->affirmEpoch ();
}

//----------------------------------------------------------------//
size_t VersionedStore::countEpochClients () const {

    assert ( this->mEpoch );
    return this->mEpoch->countClients ();
}

//----------------------------------------------------------------//
size_t VersionedStore::countEpochLayers () const {

    assert ( this->mEpoch );
    return this->mEpoch->countLayers ();
}

//----------------------------------------------------------------//
const void* VersionedStore::getRaw ( string key, size_t typeID ) const {

    assert ( this->mEpoch );
    const AbstractValueStack* valueStack = this->mEpoch->findValueStack ( key, this->mVersion );
    
    if ( valueStack ) {
        assert ( valueStack->mTypeID == typeID );
        return valueStack->getRaw ( this->mVersion );
    }
    return NULL;
}

//----------------------------------------------------------------//
bool VersionedStore::hasValue ( string key ) const {

    assert ( this->mEpoch );
    const AbstractValueStack* valueStack = this->mEpoch->findValueStack ( key, this->mVersion );
    
    if ( valueStack ) {
        return ( valueStack->getRaw ( this->mVersion ) != NULL );
    }
    return false;
}

//----------------------------------------------------------------//
void VersionedStore::popVersion () {

    assert ( this->mEpoch );

    VersionedStoreDownstream downstream = this->mEpoch->countDownstream ( this->mVersion );
    assert ( downstream.mTotal > 0 );

    if ((( downstream.mDependents == 0 ) && ( this->mEpoch->countLayers () > 1 ))) {
        this->mEpoch->popLayer ();
    }
    
    if ( this->mVersion == this->mEpoch->mVersion ) {
        this->setEpoch ( this->mEpoch->mEpoch );
    }
    
    this->mVersion--;
    this->affirmEpoch ();
}

//----------------------------------------------------------------//
void VersionedStore::prepareForSetValue () {

    assert ( this->mEpoch );

    VersionedStoreDownstream downstream = this->mEpoch->countDownstream ( this->mVersion );
    assert ( downstream.mTotal > 0 );

    if ( downstream.mTotal > 1 ) {
        this->setEpoch ( make_shared < VersionedStoreEpoch >( this->mEpoch, this->mVersion ));
    }
}

//----------------------------------------------------------------//
void VersionedStore::pushVersion () {

    assert ( this->mEpoch );

    VersionedStoreDownstream downstream = this->mEpoch->countDownstream ( this->mVersion );
    assert ( downstream.mTotal > 0 );

    this->mVersion++;

    // if more clients, can't change this epoch - gotta make a new one
    if ( downstream.mDependents > 0 ) {

        // make and set a new epoch
        shared_ptr < VersionedStoreEpoch > epoch = make_shared < VersionedStoreEpoch >();
        epoch->setEpoch ( this->mEpoch );
        epoch->mVersion = this->mVersion;
        this->setEpoch ( epoch );
    }
    else {
    
        // make a new layer in the current epoch
        this->mEpoch->pushLayer ();
    }
}

//----------------------------------------------------------------//
void VersionedStore::setRaw ( string key, size_t typeID, const void* value ) {

    assert ( this->mEpoch );
    assert ( this->mEpoch->mLayers.size () > 0 );

    this->mEpoch->mLayers.back ()->insert ( key );

    AbstractValueStack* valueStack = this->mEpoch->mValueStacksByKey [ key ].get ();
    assert ( valueStack );

    valueStack->pushBackRaw ( value, this->mVersion );
}

//----------------------------------------------------------------//
void VersionedStore::takeSnapshot ( VersionedStore& other ) {

    this->setEpoch ( other.mEpoch );
    this->mVersion = other.mVersion;
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
}

} // namespace Volition
