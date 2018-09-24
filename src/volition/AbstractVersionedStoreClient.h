// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTVERSIONEDSTORECLIENT_H
#define VOLITION_ABSTRACTVERSIONEDSTORECLIENT_H

#include <volition/common.h>

namespace Volition {

class VersionedStoreBranch;

//================================================================//
// AbstractVersionedStoreClient
//================================================================//
class AbstractVersionedStoreClient {
protected:

    friend class VersionedStoreBranch;

    shared_ptr < VersionedStoreBranch >     mBranch;

    //----------------------------------------------------------------//
    bool                canJoin                                                 () const;
    size_t              getJoinScore                                            () const;
    size_t              getVersionDependency                                    () const;
    void                joinBranch                                              ( VersionedStoreBranch& branch );
    bool                preventJoin                                             () const;

    //----------------------------------------------------------------//
    virtual bool        AbstractVersionedStoreClient_canJoin                    () const = 0;
    virtual size_t      AbstractVersionedStoreClient_getJoinScore               () const = 0;
    virtual size_t      AbstractVersionedStoreClient_getVersionDependency       () const = 0;
    virtual void        AbstractVersionedStoreClient_joinBranch                 ( VersionedStoreBranch& branch ) = 0;
    virtual bool        AbstractVersionedStoreClient_preventJoin                () const = 0;

public:

    //----------------------------------------------------------------//
                        AbstractVersionedStoreClient                            ();
    virtual             ~AbstractVersionedStoreClient                           ();
};

} // namespace Volition
#endif
