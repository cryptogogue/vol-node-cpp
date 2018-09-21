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
        this->mEpoch = make_shared < VersionedStoreEpoch >();
        this->mVersion = 0;
    }
}

//----------------------------------------------------------------//
void VersionedStore::clear () {

    this->setEpoch ( NULL, 0 );
}

//----------------------------------------------------------------//
const void* VersionedStore::getRaw ( string key, size_t typeID ) const {

    return this->mEpoch ? this->mEpoch->getRaw ( this->mVersion, key, typeID ) : NULL;

//    if ( this->mEpoch ) {
//    
//        const AbstractValueStack* valueStack = this->mEpoch->findValueStack ( key, this->mVersion );
//
//        if ( valueStack ) {
//            assert ( valueStack->mTypeID == typeID );
//            return valueStack->getRaw ( this->mVersion );
//        }
//    }
//    return NULL;
}

//----------------------------------------------------------------//
size_t VersionedStore::getVersion () const {

    return this->mVersion;
}

//----------------------------------------------------------------//
//bool VersionedStore::hasValue ( string key ) const {
//
//    if ( this->mEpoch ) {
//        const AbstractValueStack* valueStack = this->mEpoch->findValueStack ( key, this->mVersion );
//        
//        if ( valueStack ) {
//            return ( valueStack->getRaw ( this->mVersion ) != NULL );
//        }
//    }
//    return false;
//}

//----------------------------------------------------------------//
void VersionedStore::popVersion () {

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

    this->affirmEpoch ();

    if (( this->mEpoch->countDependencies () - 1 ) > 0 ) {
        
        size_t immutableTop = this->mEpoch->findImmutableTop ( this );
        
        if ( this->mVersion <= immutableTop ) {
            this->mEpoch->eraseClient ( *this );
            this->mEpoch = make_shared < VersionedStoreEpoch >( this->mEpoch, this->mVersion );
            this->mEpoch->affirmClient ( *this );
        }
    }
}

//----------------------------------------------------------------//
void VersionedStore::pushVersion () {

    if ( !this->mEpoch ) {
        this->mVersion = 0;
    }

    this->affirmEpoch ();
    assert ( this->mEpoch );

    this->mVersion++;
    
    if ( this->mVersion < this->mEpoch->mTopVersion ) {
        this->mEpoch->eraseClient ( *this );
        this->mEpoch = make_shared < VersionedStoreEpoch >( this->mEpoch, this->mVersion - 1 );
        this->mEpoch->affirmClient ( *this );
    }
}

//----------------------------------------------------------------//
void VersionedStore::rewind ( size_t version ) {

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

    if ( this->mEpoch != epoch ) {
        
        if ( this->mEpoch ) {
            this->mEpoch->eraseClient ( *this );
        }
        
        this->mEpoch = epoch;
        
        if ( epoch ) {
            epoch->affirmClient ( *this );
            assert ( version >= epoch->mBaseVersion );
        }
    }
    
    this->mVersion = this->mEpoch ? version : 0;
}

//----------------------------------------------------------------//
void VersionedStore::setRaw ( string key, const void* value ) {

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

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
//size_t VersionedStore::AbstractVersionedStoreEpochClient_getBaseVersion () const {
//
//    return this->mVersion;
//}

} // namespace Volition
