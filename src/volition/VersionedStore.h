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
// AbstractValueStack
//================================================================//
class AbstractValueStack {
protected:

    friend class VersionedStore;
    friend class VersionedStoreEpoch;
    template < typename TYPE > friend class ValueStack;

    size_t              mTypeID;
    vector < size_t >   mVersions;

    //----------------------------------------------------------------//
    const void*                             getRaw                                  () const;
    bool                                    isEmpty                                 () const;
    unique_ptr < AbstractValueStack >       makeEmptyCopy                           () const;
    void                                    pop                                     ();
    void                                    pushBackRaw                             ( const void* value );
    void                                    pushFrontRaw                            ( const void* value );

    //----------------------------------------------------------------//
    virtual const void*                             AbstractValueStack_getRaw           () const = 0;
    virtual bool                                    AbstractValueStack_isEmpty          () const = 0;
    virtual unique_ptr < AbstractValueStack >       AbstractValueStack_makeEmptyCopy    () const = 0;
    virtual void                                    AbstractValueStack_pop              () = 0;
    virtual void                                    AbstractValueStack_pushBackRaw      ( const void* value ) = 0;
    virtual void                                    AbstractValueStack_pushFrontRaw     ( const void* value ) = 0;

public:

    //----------------------------------------------------------------//
                        AbstractValueStack                  ();
    virtual             ~AbstractValueStack                 ();
};

//================================================================//
// ValueStack
//================================================================//
template < typename TYPE >
class ValueStack :
    public AbstractValueStack {
protected:

    friend class VersionedStore;
    friend class VersionedStoreLayer;
    friend class VersionedStoreEpoch;

    vector < TYPE >     mValues;

    //----------------------------------------------------------------//
    const void* AbstractValueStack_getRaw () const override {
        assert ( this->mValues.size () > 0 );
        return &this->mValues.back ();
    }
    
    //----------------------------------------------------------------//
    bool AbstractValueStack_isEmpty () const override {
        return ( this->mValues.size () == 0 );
    }
    
    //----------------------------------------------------------------//
    unique_ptr < AbstractValueStack > AbstractValueStack_makeEmptyCopy () const override {
        return make_unique < ValueStack < TYPE >>();
    }
    
    //----------------------------------------------------------------//
    void AbstractValueStack_pop () override {
        assert ( this->mValues.size () > 0 );
        this->mValues.pop_back ();
    }
    
    //----------------------------------------------------------------//
    void AbstractValueStack_pushBackRaw ( const void* value ) override {
        assert ( value );
        this->mValues.push_back ( *( const TYPE* )value );
    }
    
    //----------------------------------------------------------------//
    void AbstractValueStack_pushFrontRaw ( const void* value ) override {
        assert ( value );
        this->mValues.insert ( this->mValues.begin (), *( const TYPE* )value );
    }

public:

    //----------------------------------------------------------------//
    ValueStack () {
        this->mTypeID = typeid ( TYPE ).hash_code ();
    }
};

//================================================================//
// VersionedStoreLayer
//================================================================//
class VersionedStoreLayer {
private:

    friend class VersionedStore;
    friend class VersionedStoreEpoch;
    
    set < string >      mKeys; // keys of values this layer changes
};

//================================================================//
// VersionedStoreEpoch
//================================================================//
class VersionedStoreEpoch :
    public enable_shared_from_this < VersionedStoreEpoch > {
private:

    friend class VersionedStore;
    
    set < VersionedStore* >                                 mClients;
    vector < unique_ptr < VersionedStoreLayer >>            mLayers;
    map < string, unique_ptr < AbstractValueStack >>        mValueStacksByKey;
    
    shared_ptr < VersionedStoreEpoch >                      mParent;
    set < VersionedStoreEpoch* >                            mChildren;

    //----------------------------------------------------------------//
    template < typename TYPE >
    void affirmValueStack ( string key ) {
        unique_ptr < AbstractValueStack >& versionedValue = this->mValueStacksByKey [ key ];
        if ( !versionedValue ) {
            versionedValue = make_unique < ValueStack < TYPE >>();
        }
    }

    //----------------------------------------------------------------//
    void                                    affirmLayer                 ();
    const AbstractValueStack*               findValueStack              ( string key ) const;
    void                                    moveChildrenTo              ( VersionedStoreEpoch& epoch );
    void                                    moveClientTo                ( VersionedStore& client, VersionedStoreEpoch* epoch );
    void                                    moveClientsTo               ( VersionedStoreEpoch& epoch, const VersionedStore* except = NULL );
    void                                    moveTopLayerTo              ( VersionedStoreEpoch& epoch );
    void                                    popLayer                    ();
    void                                    pushLayer                   ();
    shared_ptr < VersionedStoreEpoch >      spawnChildEpoch             ();

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
    friend class VersionedStoreEpoch;
    
    shared_ptr < VersionedStoreEpoch >  mEpoch;

    //----------------------------------------------------------------//
    VersionedStore& operator = ( const VersionedStore& other ) {
        assert ( false );
    }

    //----------------------------------------------------------------//
    void            clear                   ();
    const void*     getRaw                  ( string key, size_t typeID ) const;
    void            setRaw                  ( string key, size_t typeID, const void* value );
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
    void setValue ( string key, const TYPE& value ) {
        assert ( this->mEpoch );
        this->mEpoch->affirmValueStack < TYPE >( key );
        this->setRaw ( key, typeid ( TYPE ).hash_code (), &value );
    }
};

} // namespace Volition
#endif
