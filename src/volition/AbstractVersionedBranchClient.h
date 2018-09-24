// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTVERSIONEDBRANCHCLIENT_H
#define VOLITION_ABSTRACTVERSIONEDBRANCHCLIENT_H

#include <volition/common.h>

namespace Volition {

class VersionedBranch;

//================================================================//
// AbstractVersionedBranchClient
//================================================================//
/** \brief Base class for VersionedBranch clients. Branch client types
    include branches (parent-to-child) as well as snapshots and iterators.
 
    Implementing this functionality as a base class saves some redundant
    code in VersionedBranch at the expense of some extra methods to implement
    in VersionedBranch and VersionedStoreSnapshot (which is the base for
    snapshots and iterators).
 
    In practice, it is easier and cleaner to implement a handful of
    single-line virtual methods than to complicate the client bookkeeping
    in VersionedBranch.
*/
class AbstractVersionedBranchClient {
protected:

    friend class VersionedBranch;

    /// The source (or parent) branch for this client. May be NULL.
    shared_ptr < VersionedBranch >      mSourceBranch;
    
    /// The current (or base) version for this client.
    size_t                              mVersion;

    //----------------------------------------------------------------//
    bool                canJoin                                                 () const;
    size_t              getJoinScore                                            () const;
    size_t              getVersionDependency                                    () const;
    void                joinBranch                                              ( VersionedBranch& branch );
    bool                preventJoin                                             () const;
    void                setBranch                                               ( shared_ptr < VersionedBranch > branch );
    void                setBranch                                               ( shared_ptr < VersionedBranch > branch, size_t version );

    //----------------------------------------------------------------//
    virtual bool        AbstractVersionedStoreClient_canJoin                    () const = 0;
    virtual size_t      AbstractVersionedStoreClient_getJoinScore               () const = 0;
    virtual size_t      AbstractVersionedStoreClient_getVersionDependency       () const = 0;
    virtual void        AbstractVersionedStoreClient_joinBranch                 ( VersionedBranch& branch ) = 0;
    virtual bool        AbstractVersionedStoreClient_preventJoin                () const = 0;

public:

    //----------------------------------------------------------------//
                        AbstractVersionedBranchClient                            ();
    virtual             ~AbstractVersionedBranchClient                           ();
};

} // namespace Volition
#endif
