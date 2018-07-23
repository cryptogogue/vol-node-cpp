// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTORE_H
#define VOLITION_VERSIONEDSTORE_H

#include <volition/common.h>
#include <volition/MinerInfo.h>

// TODO: this is all placeholder stuff, to get the algorithm working. will need to
// optimize to reduce dynamic allocation. will also need to provide a NoSQL-backed
// implementation.

namespace Volition {

class VersionedStore;

//================================================================//
// AbstractVersionedValue
//================================================================//
class AbstractVersionedValue {
protected:

    friend class VersionedStore;
    friend class VersionedStoreEpoch;
    template < typename TYPE > friend class VersionedValue;

    type_info           mTypeID;
    vector < size_t >   mVersions;

    //----------------------------------------------------------------//
    unique_ptr < AbstractVersionedValue >   copyTop                                 () const;
    void*                                   getRaw                                  () const;
    bool                                    isEmpty                                 () const;
    void                                    pop                                     ();
    void                                    setRaw                                  ( const void* value );

    //----------------------------------------------------------------//
    virtual unique_ptr < AbstractVersionedValue >       AbstractVersionedValue_copyTop          () const = 0;
    virtual void*                                       AbstractVersionedValue_getRaw           () const = 0;
    virtual bool                                        AbstractVersionedValue_isEmpty          () const = 0;
    virtual void                                        AbstractVersionedValue_pop              () = 0;
    virtual void                                        AbstractVersionedValue_setRaw           ( const void* value ) = 0;

public:

    //----------------------------------------------------------------//
    virtual             ~AbstractVersionedValue                 ();
};

//================================================================//
// VersionedValue
//================================================================//
template < typename TYPE >
class VersionedValue :
    AbstractVersionedValue {
protected:

    friend class VersionedStore;
    friend class VersionedStoreLayer;
    friend class VersionedStoreEpoch;

    vector < TYPE >     mValues;

    //----------------------------------------------------------------//
    unique_ptr < AbstractVersionedValue > AbstractVersionedValue_copyTop () const override {
    
        unique_ptr < VersionedValue < TYPE >> value = make_unique < VersionedValue < TYPE >>();
        
        assert ( this->mValues.size ());
        value->mValues.push_back ( this->mValues.back ());
        return move ( value );
    }

    //----------------------------------------------------------------//
    void* AbstractVersionedValue_getRaw () const override {
    
        assert ( this->mValues.size () > 0 );
        return &this->mValues.back ();
    }
    
    //----------------------------------------------------------------//
    bool AbstractVersionedValue_isEmpty () const override {
        return ( this->mValues.size () == 0 );
    }
    
    //----------------------------------------------------------------//
    void AbstractVersionedValue_pop () override {
    
        assert ( this->mValues.size () > 0 );
        this->mValues.pop_back ();
    }
    
    //----------------------------------------------------------------//
    void AbstractVersionedValue_setRaw ( const void* value ) override {
    
        assert ( value );
        this->mValues.push_back ( *( const TYPE* )value );
    }
    
    //----------------------------------------------------------------//
    VersionedValue () {
        this->mTypeID = typeid ( TYPE );
    }
};

//================================================================//
// VersionedStoreLayer
//================================================================//
class VersionedStoreLayer {
private:

    friend class VersionedStore;
    
    set < string >      mKeys; // keys of values this layer changes
};

//================================================================//
// VersionedStoreEpoch
//================================================================//
class VersionedStoreEpoch {
private:

    friend class VersionedStore;
    
    set < VersionedStore* >                                 mClients;
    vector < unique_ptr < VersionedStoreLayer >>            mLayers;
    map < string, unique_ptr < AbstractVersionedValue >>    mMap;
    
    shared_ptr < VersionedStoreEpoch >                      mParent;
    set < VersionedStoreEpoch* >                            mChildren;

    //----------------------------------------------------------------//
    const void*     getRaw                      ( string key ) const;
    void            setRaw                      ( string key, const void* value );

    //----------------------------------------------------------------//
    template < typename TYPE >
    bool checkValue ( string key ) const {
        map < string, unique_ptr < AbstractVersionedValue >> ::const_iterator mapIt = this->mMap.find ( key );
        if ( mapIt != this->mMap.cend ()) {
            return (( mapIt->second ) && ( mapIt->second->mTypeID == typeid ( TYPE )));
        }
        return false;
    }
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    const TYPE* getValue ( string key ) const {

        assert ( this->checkValue < TYPE >( key ));
        return ( TYPE* )this->getRaw ( key );
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void setValue ( string key, const TYPE& value ) {

        unique_ptr < AbstractVersionedValue >& versionedValue = this->mMap [ key ];
        if ( !versionedValue ) {
            versionedValue = make_unique < VersionedValue < TYPE >>();
        }
        this->setRaw ( key, &value );
    }

public:

    //----------------------------------------------------------------//
            VersionedStoreEpoch         ();
            ~VersionedStoreEpoch        ();
};

//================================================================//
// VersionedStore
//================================================================//
class VersionedStore {
private:

    friend class VersionedStore;
    
    shared_ptr < VersionedStoreEpoch >  mEpoch;

    //----------------------------------------------------------------//
    VersionedStore& operator = ( const VersionedStore& other ) {
        assert ( false );
    }

    //----------------------------------------------------------------//
    void            clear                   ();
    void            takeSnapshot            ( VersionedStore& other );
                    VersionedStore          ( const VersionedStore& other );
public:

    //----------------------------------------------------------------//
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
    const TYPE* getValue ( string key ) const {
        return this->mEpoch->getValue < TYPE >( key );
    }

    //----------------------------------------------------------------//
    template < typename TYPE >
    void setValue ( string key, const TYPE& value ) {
        this->mEpoch->setValue < TYPE >( key, value );
    }
};

} // namespace Volition
#endif
