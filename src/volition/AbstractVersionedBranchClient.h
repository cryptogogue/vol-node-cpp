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
class AbstractVersionedBranchClient {
protected:

    friend class VersionedBranch;

    shared_ptr < VersionedBranch >      mBranch;
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
