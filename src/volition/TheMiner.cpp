// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include "AbstractHashable.h"
#include "Block.h"
#include "Genesis.h"
#include "SyncChainTask.h"
#include "TheMiner.h"

namespace Volition {

//================================================================//
// TheMiner
//================================================================//

//----------------------------------------------------------------//
const Chain* TheMiner::getChain () const {

    return this->mChain ? this->mChain.get () : 0;
}

//----------------------------------------------------------------//
string TheMiner::getPublicKey () {

    stringstream strStream;
    this->mKeyPair->save ( &strStream );
    return strStream.str ();
}

//----------------------------------------------------------------//
void TheMiner::loadGenesis ( string genesis ) {
    
    fstream inStream;
    inStream.open ( genesis, ios_base::in );
    
    unique_ptr < Block > block = make_unique < Block >();
    block->fromJSON ( inStream );

    this->mState = State ();
    
    unique_ptr < Chain > chain = make_unique < Chain >( move ( block ));
    
    if ( chain->verify ( this->mState )) {
    
        this->mChain = move ( chain );
        this->mChain->apply ( this->mState );
    }
}

//----------------------------------------------------------------//
void TheMiner::loadKey ( string keyfile, string password ) {

    this->mKeyPair = make_unique < Poco::Crypto::ECKey >( "", keyfile, password );
}

//----------------------------------------------------------------//
void TheMiner::onSyncChainNotification ( Poco::TaskFinishedNotification* pNf ) {

    SyncChainTask* task = dynamic_cast < SyncChainTask* >( pNf->task ());
    if ( task ) {
        this->onSyncChainNotification ( *task );
    }
    pNf->release ();
}

//----------------------------------------------------------------//
void TheMiner::onSyncChainNotification ( SyncChainTask& task ) {

    string minerID      = task.getMinerID ();
    string url          = task.getURL ();
    
    assert ( this->mMinerURLs.find ( minerID ) != this->mMinerURLs.end ());
    this->mMinerURLs.erase ( minerID );
    
    unique_ptr < Chain > chain = task.moveChain ();
    if ( chain ) {
        
        chain->verify ( this->mState ); // TODO: move to task thread; use snapshot of state when task created
        
        this->pushBlock ( *chain, false );
        
        if ( this->mChain.get () != Chain::choose ( *this->mChain, *chain )) {
            this->mChain = move ( chain );
        }
        this->mChain->print ();
    }
}

//----------------------------------------------------------------//
void TheMiner::pushBlock ( Chain& chain, bool force ) {

    ChainPlacement placement = chain.findPlacement ( this->mMinerID, force );
    if ( placement.canPush ()) {
        unique_ptr < Block > block = make_unique < Block >();
        chain.pushAndSign ( placement, move ( block ), *this->mKeyPair );
    }
}

//----------------------------------------------------------------//
void TheMiner::pushTransaction ( unique_ptr < AbstractTransaction >& transaction ) {

    this->mPendingTransactions.push_back ( move ( transaction ));
}

//----------------------------------------------------------------//
void TheMiner::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
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

    size_t count = 0;
    while ( !this->isStopped ()) {
    
        if ( this->mChain && ( this->mMinerURLs.size () == 0 )) {
            
            this->pushBlock ( *this->mChain, true );
            this->mChain->print ();
            
            this->mMinerURLs = this->mState.getMinerURLs ();
            
            if ( this->mMinerURLs.find ( this->mMinerID ) != this->mMinerURLs.end ()) {
                this->mMinerURLs.erase ( this->mMinerID );
            }
            
            map < string, string >::iterator urlIt = this->mMinerURLs.begin ();
            for ( ; urlIt != this->mMinerURLs.end (); ++urlIt ) {
                this->mTaskManager.start ( new SyncChainTask ( urlIt->first, urlIt->second ));
            }
        }
        printf ( "%d\n", ( unsigned int )count++ );
        Poco::Thread::sleep ( 200 );
    }
}

} // namespace Volition
