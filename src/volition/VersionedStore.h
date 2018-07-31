// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTORE_H
#define VOLITION_VERSIONEDSTORE_H

#include <volition/common.h>
#include <volition/VersionedStoreEpoch.h>
#include <volition/VersionedStoreEpochClient.h>

// TODO: this is all placeholder stuff, to get the algorithm working. will need to
// optimize to reduce dynamic allocation. will also need to provide a NoSQL-backed
// implementation.

namespace Volition {

//================================================================//
// VersionedStore
//================================================================//
class VersionedStore :
    public VersionedStoreEpochClient {
private:

    friend class VersionedStore;
    friend class VersionedStoreEpoch;

    //----------------------------------------------------------------//
    VersionedStore& operator = ( const VersionedStore& other ) {
        assert ( false );
    }

    //----------------------------------------------------------------//
    void            affirmEpoch             ();
    const void*     getRaw                  ( string key, size_t typeID ) const;
    void            prepareForSetValue      ();
    void            setRaw                  ( string key, size_t typeID, const void* value );
    void            takeSnapshot            ( VersionedStore& other );
                    VersionedStore          ( const VersionedStore& other );
    
public:

    //----------------------------------------------------------------//
    void            clear                   ();
    size_t          countEpochClients       () const;
    size_t          countEpochLayers        () const;
    bool            hasValue                ( string key ) const;
    void            popVersion              ();
    void            pushVersion             ();
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
    const TYPE getValue ( string key ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key );
        assert ( value );
        return *value;
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    const TYPE* getValueOrNil ( string key ) const {
        return ( TYPE* )this->getRaw ( key, typeid ( TYPE ).hash_code ());
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
        this->setRaw ( key, typeid ( TYPE ).hash_code (), &value );
    }
};

} // namespace Volition
#endif
