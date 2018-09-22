// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStore.h>

//#define DEBUG_LOG printf ( "%04x:  ", ( int )(( size_t )this ) & 0xffff ); printf
#define DEBUG_LOG(...)

namespace Volition {

//================================================================//
// VersionedStore
//================================================================//

//----------------------------------------------------------------//
void VersionedStore::affirmEpoch () {

    if ( !this->mEpoch ) {
        assert ( this->mVersion == 0 );
        this->setEpoch ( make_shared < VersionedStoreEpoch >(), this->mVersion );
    }
}

//----------------------------------------------------------------//
void VersionedStore::clear () {

    this->setEpoch ( NULL, 0 );
}

//----------------------------------------------------------------//
const void* VersionedStore::getRaw ( string key, size_t version, size_t typeID ) const {

    return this->mEpoch ? this->mEpoch->getRaw ( version < this->mVersion ? version : this->mVersion, key, typeID ) : NULL;
}

//----------------------------------------------------------------//
size_t VersionedStore::getVersion () const {

    return this->mVersion;
}

//----------------------------------------------------------------//
size_t VersionedStore::getVersionDependency () const {

    return this->mVersion + 1;
}

//----------------------------------------------------------------//
void VersionedStore::popVersion () {

    LOG_SCOPE_F ( INFO, "VersionedStore::  ()" );

    if ( this->mEpoch ) {
    
        size_t version = this->mVersion;
        shared_ptr < VersionedStoreEpoch > epoch = this->mEpoch;
        epoch->eraseClient ( *this );
        
        this->mEpoch = NULL;
        this->mVersion = 0;
        
        if ( version == epoch->mBaseVersion ) {
            epoch = epoch->mParent;
        }
        
        if ( epoch ) {
            epoch->affirmClient ( *this );
            this->mEpoch = epoch;
            this->mVersion = version - 1;
            this->mEpoch->optimize ();
        }
    }
}

//----------------------------------------------------------------//
void VersionedStore::prepareForSetValue () {

    LOG_SCOPE_F ( INFO, "VersionedStore::prepareForSetValue ()" );

    this->affirmEpoch ();
    
    size_t dependencies = this->mEpoch->countDependencies ();
    if ( dependencies > 1 ) {
    
        size_t immutableTop = this->mEpoch->findImmutableTop ( this );
        LOG_F ( INFO, "immutableTop: %d", ( int )immutableTop );
        
        if ( this->mVersion < immutableTop ) {
        
            LOG_F ( INFO, "SPLIT!" );
        
            shared_ptr < VersionedStoreEpoch > epoch = this->mEpoch;
            this->mEpoch->eraseClient ( *this );
            this->mEpoch = make_shared < VersionedStoreEpoch >( this->mEpoch, this->mVersion );
            this->mEpoch->affirmClient ( *this );
            epoch->optimize ();
        }
    }
}

//----------------------------------------------------------------//
void VersionedStore::pushVersion () {

    LOG_SCOPE_F ( INFO, "VersionedStore::pushVersion ()" );

    if ( !this->mEpoch ) {
        this->mVersion = 0;
    }

    this->affirmEpoch ();
    assert ( this->mEpoch );

    this->mVersion++;
    LOG_F ( INFO, "version: %d", ( int )this->mVersion );
    
    if ( this->mVersion < this->mEpoch->getTopVersion ()) {
    
        LOG_F ( INFO, "SPLIT" );
    
        shared_ptr < VersionedStoreEpoch > epoch = this->mEpoch;
        this->mEpoch->eraseClient ( *this );
        this->mEpoch = make_shared < VersionedStoreEpoch >( this->mEpoch, this->mVersion - 1 );
        this->mEpoch->affirmClient ( *this );
        epoch->optimize ();
    }
}

//----------------------------------------------------------------//
void VersionedStore::rewind ( size_t version ) {

    LOG_SCOPE_F ( INFO, "VersionedStore::rewind ( %d )", ( int )version );

    assert ( version <= this->mVersion );
    
    if (( this->mEpoch ) && ( version < this->mVersion )) {
    
        shared_ptr < VersionedStoreEpoch > epoch = this->mEpoch;
        epoch->eraseClient ( *this );
        this->mEpoch = NULL;
        
        size_t top = this->mVersion;
        for ( ; epoch && ( version < epoch->mBaseVersion ); epoch = epoch->mParent ) {
            top = epoch->mBaseVersion;
        }
        
        assert ( epoch );
        assert ( epoch->mBaseVersion <= version );
        assert ( version < top );
        
        epoch->affirmClient ( *this );
        this->mEpoch = epoch;
        this->mVersion = version;
        this->mEpoch->optimize ();
    }
}

//----------------------------------------------------------------//
void VersionedStore::setDebugName ( string debugName ) {

    #ifdef _DEBUG
        this->mDebugName = debugName;
    #endif
}

//----------------------------------------------------------------//
void VersionedStore::setEpoch ( shared_ptr < VersionedStoreEpoch > epoch, size_t version ) {

    weak_ptr < VersionedStoreEpoch > prevEpochWeak;

    if ( this->mEpoch != epoch ) {
        
        LOG_SCOPE_F ( INFO, "VersionedStore::setEpoch () - changing epoch" );
        
        if ( this->mEpoch ) {
            prevEpochWeak = this->mEpoch;
            this->mEpoch->eraseClient ( *this );
        }
        
        this->mEpoch = epoch;
        epoch = NULL;
    }
    
     if ( this->mEpoch ) {
        assert ( version >= this->mEpoch->mBaseVersion );
        this->mVersion = version;
        this->mEpoch->affirmClient ( *this );
        this->mEpoch->optimize ();
    }
    else {
        this->mVersion = 0;
    }

    if ( !prevEpochWeak.expired ()) {
        epoch = prevEpochWeak.lock ();
        epoch->optimize ();
    }
}

//----------------------------------------------------------------//
void VersionedStore::setRaw ( string key, const void* value ) {

    LOG_SCOPE_F ( INFO, "VersionedStore::setRaw ( %s, %p )", key.c_str (), value );

    assert ( this->mEpoch );
    this->mEpoch->setRaw ( this->mVersion, key, value );
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

    weak_ptr < VersionedStoreEpoch > epochWeak = this->mEpoch;
    this->setEpoch ( NULL, 0 );
    if ( !epochWeak.expired ()) {
        shared_ptr < VersionedStoreEpoch > epoch = epochWeak.lock ();
        epoch->optimize ();
    }
}

} // namespace Volition
