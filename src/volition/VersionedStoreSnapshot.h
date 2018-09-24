// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONEDSTORESNAPSHOT_H
#define VOLITION_VERSIONEDSTORESNAPSHOT_H

#include <volition/common.h>
#include <volition/AbstractVersionedStoreClient.h>
#include <volition/VersionedStoreBranch.h>

namespace Volition {

//================================================================//
// VersionedStoreSnapshot
//================================================================//
/** \brief Common base implementation for versioned storeiterators. Contains only read-only methods
    and getters.
 
    This is the base to VersionedStore, VersionedStoreIterator and VersionedValueIterator. It
    was originally part of VersionedStore, but was broken out into a separate base class
    to avoid exposing mutators through the interator implementations, which are read-only.
*/
class VersionedStoreSnapshot :
    public AbstractVersionedStoreClient {
protected:

    friend class AbstractVersionedValueIterator;
    friend class VersionedStoreBranch;
    friend class VersionedStoreIterator;
    template < typename > friend class VersionedValue;
    template < typename > friend class VersionedValueIterator;
    
    /// Current version of the store. Values set will be set at this version.
    size_t                                  mVersion;

    #ifdef _DEBUG
        /// Available in debug builds to add an easily readable name to snapshots.
        string                              mDebugName;
    #endif

    //----------------------------------------------------------------//
    void            affirmBranch                    ();
    const void*     getRaw                          ( string key, size_t version, size_t typeID ) const;
    void            setBranch                       ( shared_ptr < VersionedStoreBranch > epoch, size_t version );
    
    //----------------------------------------------------------------//
    /** \brief  Return a pointer to the value for a key at a given version
                or NULL if the value cannot be found.
     
                Returns the value for the most recent version equal to or less than
                the given version.
     
        \param  key         The key.
        \param  version     The version.
        \return             A pointer to the value or NULL.
    */
    template < typename TYPE >
    const TYPE* getValueOrNil ( string key, size_t version ) const {
        return ( TYPE* )this->getRaw ( key, version, typeid ( TYPE ).hash_code ());
    }
    
    //----------------------------------------------------------------//
    bool            AbstractVersionedStoreClient_canJoin                    () const override;
    size_t          AbstractVersionedStoreClient_getJoinScore               () const override;
    size_t          AbstractVersionedStoreClient_getVersionDependency       () const override;
    void            AbstractVersionedStoreClient_joinBranch                 ( VersionedStoreBranch& branch ) override;
    bool            AbstractVersionedStoreClient_preventJoin                () const override;
    
public:

    //----------------------------------------------------------------//
    void            clear                           ();
    size_t          getVersion                      () const;
    void            setDebugName                    ( string debugName );
    void            takeSnapshot                    ( VersionedStoreSnapshot& other );
                    VersionedStoreSnapshot          ();
                    VersionedStoreSnapshot          ( VersionedStoreSnapshot& other );
    virtual         ~VersionedStoreSnapshot         ();
    
    //----------------------------------------------------------------//
    /** \brief  Implements assignment by calling takeSnapshot().
     
        \param  other   The version to snapshot.
    */
    VersionedStoreSnapshot& operator = ( VersionedStoreSnapshot& other ) {
        this->takeSnapshot ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    /** \brief  Return a copy of the value for a key. Throws an exception if the
                value cannot be found.
     
        \param  key     The key.
        \return         A copy of the value.
    */
    template < typename TYPE >
    const TYPE getValue ( string key ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key, this->mVersion );
        assert ( value );
        return *value;
    }

    //----------------------------------------------------------------//
    /** \brief  Return a copy of the value for a key at a given version.
                Throws an exception if the value cannot be found.
     
                Returns the value for the most recent version equal to or less than
                the given version.
     
        \param  key         The key.
        \param  version     The version.
        \return             A copy of the value.
    */
    template < typename TYPE >
    const TYPE getValue ( string key, size_t version ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key, version );
        assert ( value );
        return *value;
    }

    //----------------------------------------------------------------//
    /** \brief  Check to see if the value can be found.
     
        \param  key     The key.
        \return         True if the value exists. False if it cant.
    */
    template < typename TYPE >
    bool hasValue ( string key ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key, this->mVersion );
        return ( value != NULL );
    }
    
    //----------------------------------------------------------------//
    /** \brief  Check to see if the value can be found for the given version.
     
                Check the value for the most recent version equal to or less than
                the given version.
     
        \param  key     The key.
        \return         True if the value exists. False if it cant.
    */
    template < typename TYPE >
    bool hasValue ( string key, size_t version ) const {
        const TYPE* value = this->getValueOrNil < TYPE >( key, version );
        return ( value != NULL );
    }
};

} // namespace Volition
#endif
