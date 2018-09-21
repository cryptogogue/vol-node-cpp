// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTORE_H
#define VOLITION_VERSIONEDSTORE_H

#include <volition/common.h>
#include <volition/VersionedStoreEpoch.h>

// TODO: this is all placeholder stuff, to get the algorithm working. will need to
// optimize to reduce dynamic allocation. will also need to provide a NoSQL-backed
// implementation.

namespace Volition {

//================================================================//
// VersionedStore
//================================================================//
class VersionedStore {
protected:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStore;
    friend class VersionedStoreEpoch;
    friend class VersionedStoreIterator;
    template < typename > friend class VersionedValueIterator;

    shared_ptr < VersionedStoreEpoch >      mEpoch;
    size_t                                  mVersion;

    #ifdef _DEBUG
        string                              mDebugName;
    #endif

    //----------------------------------------------------------------//
    void            affirmEpoch             ();
    const void*     getRaw                  ( string key, size_t version, size_t typeID ) const;
    size_t          getVersionDependency    () const;
    void            prepareForSetValue      ();
    void            setEpoch                ( shared_ptr < VersionedStoreEpoch > epoch, size_t version );
    void            setRaw                  ( string key, const void* value );
    
public:

    //----------------------------------------------------------------//
    void            clear                   ();
    size_t          getVersion              () const;
    void            popVersion              ();
    void            pushVersion             ();
    void            rewind                  ( size_t version );
    void            setDebugName            ( string debugName );
    void            takeSnapshot            ( VersionedStore& other );
                    VersionedStore          ();
                    VersionedStore          ( VersionedStore& other );
                    ~VersionedStore         ();
    
    //----------------------------------------------------------------//
    VersionedStore& operator = ( VersionedStore& other ) {
        this->takeSnapshot ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    const TYPE& getValue ( string key ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key );
        assert ( value );
        return *value;
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    const TYPE& getValue ( string key, size_t version ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key, version );
        assert ( value );
        return *value;
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    const TYPE* getValueOrNil ( string key ) const {
        return ( TYPE* )this->getRaw ( key, this->mVersion, typeid ( TYPE ).hash_code ());
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    const TYPE* getValueOrNil ( string key, size_t version ) const {
        return ( TYPE* )this->getRaw ( key, version, typeid ( TYPE ).hash_code ());
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    bool hasTypedValue ( string key ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key );
        return ( value != NULL );
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void setValue ( string key, const TYPE& value ) {
        this->prepareForSetValue ();
        this->mEpoch->affirmValueStack < TYPE >( key );
        this->setRaw ( key, &value );
    }
};

} // namespace Volition
#endif
