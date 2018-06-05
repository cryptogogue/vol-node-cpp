// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THEMINER_H
#define VOLITION_THEMINER_H

#include "common.h"
#include "AbstractTransaction.h"
#include "Singleton.h"

namespace Volition {

class AbstractHashable;
class Signable;

//================================================================//
// TheMiner
//================================================================//
class TheMiner :
    public Singleton < TheMiner >,
    public Poco::Activity < TheMiner >  {
private:

    unique_ptr < Poco::Crypto::ECKey >              mKey;
    list < unique_ptr < AbstractTransaction >>      mPendingTransactions;

    Poco::TaskManager                               mTaskManager;

    //----------------------------------------------------------------//
    void            onSyncChainNotification     ( Poco::TaskFinishedNotification* pNf );
    void            run                         () override;

public:

    //----------------------------------------------------------------//
    void            loadKey                 ( string keyfile, string password = "" );
    string          getPublicKey            ();
    void            pushTransaction         ( unique_ptr < AbstractTransaction >& transaction );
    void            sign                    ( Signable& signable ) const;
    void            shutdown                ();
                    TheMiner                ();
                    ~TheMiner               ();
};

} // namespace Volition
#endif
