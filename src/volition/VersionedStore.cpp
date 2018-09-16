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
}

//----------------------------------------------------------------//
size_t VersionedStore::countEpochClients () const {

    return this->mEpoch ? this->mEpoch->countClients () : 0;
}

//----------------------------------------------------------------//
size_t VersionedStore::countEpochLayers () const {

    return this->mEpoch ? this->mEpoch->countLayers () : 0;
}

//----------------------------------------------------------------//
const void* VersionedStore::getRaw ( string key, size_t typeID ) const {

    if ( this->mEpoch ) {
        const AbstractValueStack* valueStack = this->mEpoch->findValueStack ( key, this->mVersion );
        
        if ( valueStack ) {
            assert ( valueStack->mTypeID == typeID );
            return valueStack->getRaw ( this->mVersion );
        }
    }
    return NULL;
}

//----------------------------------------------------------------//
bool VersionedStore::hasValue ( string key ) const {

    if ( this->mEpoch ) {
        const AbstractValueStack* valueStack = this->mEpoch->findValueStack ( key, this->mVersion );
        
        if ( valueStack ) {
            return ( valueStack->getRaw ( this->mVersion ) != NULL );
        }
    }
    return false;
}

//----------------------------------------------------------------//
void VersionedStore::popVersion () {

    if ( this->mEpoch ) {

        assert ( this->mEpoch->countLayers () > 0 );

        VersionedStoreDownstream downstream = this->mEpoch->countDownstream ( this->mVersion );
        assert ( downstream.mTotal > 0 );

        if ( downstream.mDependents == 0 ) {
            this->mEpoch->popLayer ();
        }
        
        if ( this->mVersion > 0 ) {
            if ( this->mVersion == this->mEpoch->mBaseVersion ) {
                this->setEpoch ( this->mEpoch->mParent, this->mVersion - 1 );
            }
            else {
                this->mVersion--;
            }
        }
        else {
            this->setEpoch ( NULL );
        }
    }
}

//----------------------------------------------------------------//
void VersionedStore::prepareForSetValue () {

    this->affirmEpoch ();
    assert ( this->mEpoch );

    VersionedStoreDownstream downstream = this->mEpoch->countDownstream ( this->mVersion );
    assert ( downstream.mTotal > 0 );

    if ( downstream.mTotal > 1 ) {
        this->setEpoch ( make_shared < VersionedStoreEpoch >( this->mEpoch, this->mVersion ));
    }
}

//----------------------------------------------------------------//
void VersionedStore::pushVersion () {

    this->affirmEpoch ();
    assert ( this->mEpoch );

    VersionedStoreDownstream downstream = this->mEpoch->countDownstream ( this->mVersion );
    assert ( downstream.mTotal > 0 );

    this->mVersion++;

    // if more clients, can't change this epoch - gotta make a new one
    if ( downstream.mDependents > 0 ) {

        // make and set a new epoch
        shared_ptr < VersionedStoreEpoch > epoch = make_shared < VersionedStoreEpoch >();
        epoch->setParent ( this->mEpoch, this->mVersion );
        this->setEpoch ( epoch );
    }
    else {
    
        // make a new layer in the current epoch
        this->mEpoch->pushLayer ();
    }
}

//----------------------------------------------------------------//
void VersionedStore::seekVersion ( size_t version ) {

    this->affirmEpoch ();

    shared_ptr < VersionedStoreEpoch > epoch = this->mEpoch;

    while ( version < epoch->mBaseVersion ) {
        epoch = epoch->mParent;
    }
    
    assert ( epoch );
    assert (( version >= epoch->mBaseVersion ) && ( version < ( epoch->mBaseVersion + epoch->mLayers.size ())));
    
    this->setEpoch ( epoch, version );
    this->mEpoch->optimize ();
}

//----------------------------------------------------------------//
void VersionedStore::setEpoch ( shared_ptr < VersionedStoreEpoch > epoch ) {

    this->setEpoch ( epoch, epoch ? epoch->getVersion () : 0 );
}

//----------------------------------------------------------------//
void VersionedStore::setEpoch ( shared_ptr < VersionedStoreEpoch > epoch, size_t version ) {

    if ( this->mEpoch != epoch ) {
        
        if ( this->mEpoch ) {
            this->mEpoch->eraseClient ( *this );
        }
        
        this->mEpoch = epoch;
        
        if ( epoch ) {
            epoch->affirmClient ( *this );
        }
    }
    // TODO: add sanity check
    this->mVersion = version;
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

    this->setEpoch ( other.mEpoch, other.mVersion );
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore () :
    mVersion ( 0 ) {
}

//----------------------------------------------------------------//
VersionedStore::VersionedStore ( VersionedStore& other ) {

    this->takeSnapshot ( other );
}

//----------------------------------------------------------------//
VersionedStore::~VersionedStore () {

    this->setEpoch ( NULL, 0 );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
size_t VersionedStore::AbstractVersionedStoreEpochClient_getVersion () const {

    return this->mVersion;
}

} // namespace Volition
