// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "AbstractHashable.h"
#include "SyncChainTask.h"
#include "TheMiner.h"

namespace Volition {

//================================================================//
// TheMiner
//================================================================//

//----------------------------------------------------------------//
void TheMiner::loadKey ( string keyfile, string password ) {

    this->mKey = make_unique < Poco::Crypto::ECKey >( "", keyfile, password );
}

//----------------------------------------------------------------//
string TheMiner::getPublicKey () {

    stringstream strStream;
    this->mKey->save ( &strStream );
    return strStream.str ();
}

//----------------------------------------------------------------//
void TheMiner::onSyncChainNotification ( Poco::TaskFinishedNotification* pNf ) {
    std::cout << pNf->task ()->name () << " finished." << std::endl;
    pNf->release ();
}

//----------------------------------------------------------------//
void TheMiner::pushTransaction ( unique_ptr < AbstractTransaction >& transaction ) {

    this->mPendingTransactions.push_back ( move ( transaction ));
}

//----------------------------------------------------------------//
void TheMiner::sign ( Signable& signable ) const {

    assert ( this->mKey );
    signable.sign ( *this->mKey, "SHA256" );
}

//----------------------------------------------------------------//
void TheMiner::shutdown () {

    this->stop ();
    this->mTaskManager.joinAll ();
    this->wait ();
}

//----------------------------------------------------------------//
TheMiner::TheMiner () :
    Poco::Activity < TheMiner >( this, &TheMiner::run ) {
    
    this->mTaskManager.addObserver (
        Poco::Observer < TheMiner, Poco::TaskFinishedNotification > ( *this, &TheMiner::onSyncChainNotification )
    );
}

//----------------------------------------------------------------//
TheMiner::~TheMiner () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void TheMiner::run () {

    size_t size = 0;
    while ( !this->isStopped ()) {
        std::cout << size++ << std::endl;
        this->mTaskManager.start ( new SyncChainTask ());
        Poco::Thread::sleep ( 200 );
    }
}

} // namespace Volition
