// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Format.h>
#include <volition/Singleton.h>
#include <volition/TheContext.h>
#include <volition/WebMiner.h>

// Axioms
// 1. SELF must agree with at least N other miners in the last M samples (where N and M may be adjusted by total active miners).
// 2. ALLURE for block N is *not* affected by previous blocks. (why not?)
// 3. there is always one (and only one) IDEAL chain. (chain with the greatest per-block allure.)
// 4. Any chain may be compared against any other and scored by their relation to the IDEAL.
//      a. It is possible for multiple chains to "tie" in scoring.
//      b. To break a tie, pick the chain with the most recent willing ALLURE.
// 5. A chain produced by more active miners will eventually beat a chain produced by fewer.
//      a. The larger pool of miners has more ALLURE to choose from, and thus is more likely to have the winning ALLUREs.

// When initially building the chain, we only need block headers (with allure).
// To compare branches, we do not have to apply any transactions.
// Once we've committed to a branch, we can request the full block.
// If a block delivery times out, discard the chain

// conceptually, always get the full chain from each node sampled
// broadcast a change as soon as detected (after evaluating batch of samples)
// only advance when consensus threshold is met and blocks are known
// roll back as soon as a better branch is proven
//      branches have to be long enough to compete
//      check headers first - if branch would lose, don't roll back
//      use waypoints to efficiently seek back to branch point
//      after seek back, gather block headers moving forward from fork
//      maintain rollback branches as competitors until proven
//      during seek, detect if branch changes and restart if unrecognized header

// always get the whole chain
// RACE AHEAD when a point of consensus is found (N of last M random samples AND blocks are known)
// REWIND as soon as a better chain is proven (and blocks are known)
// AS SOON as a new leaf is found (with an unknown root), seek back to fork then rebuild (and evaluate)
// ignore shorter branches that CANNOT WIN

// if there is an OUTAGE (more than a certain number of nodes stop responding), network should HALT

// when adding or removing a miner a hard FORK is created
// the name of the FORK must be unique and be adopted by the remaining miners

// REMOTE MINERs cache chain FRAGMENTs
// ask REMOTE MINER for FRAGMENT starting at HEIGHT
// FRAGMENT may be incomplete
// if FRAGMENT doesn't have a KNOWN ROOT, remote chain has to be rewound

// we'll keep a tree of block headers that may or may not have full blocks attached
// as chain fragments come in, we'll add them to the tree and mark branches for evaluation
// active miners have to "pin" tree branches; prune branches with no miners
// miners get updated from the service response queue

namespace Volition {

//================================================================//
// WebMiner
//================================================================//

//----------------------------------------------------------------//
SerializableTime WebMiner::getStartTime () {

    return this->mStartTime;
}

//----------------------------------------------------------------//
void WebMiner::runActivity () {

    while ( !this->isStopped ()) {
    
        Poco::Timestamp timestamp;
        
        this->step ();
        
        u32 elapsedMillis = ( u32 )( timestamp.elapsed () / 1000 );
        u32 updateMillis = this->mUpdateIntervalInSeconds * 1000;
        
        if ( elapsedMillis < updateMillis ) {
            Poco::Thread::sleep ( updateMillis - elapsedMillis );
        }
        Poco::Thread::sleep ( 5000 );
    }
}

//----------------------------------------------------------------//
void WebMiner::setUpdateInterval ( u32 updateIntervalInSeconds ) {

    this->mUpdateIntervalInSeconds = updateIntervalInSeconds;
}

//----------------------------------------------------------------//
void WebMiner::waitForShutdown () {

    this->mShutdownEvent.wait ();
}

//----------------------------------------------------------------//
WebMiner::WebMiner () :
    Poco::Activity < WebMiner >( this, &WebMiner::runActivity ),
    mUpdateIntervalInSeconds ( DEFAULT_UPDATE_INTERVAL ) {
}

//----------------------------------------------------------------//
WebMiner::~WebMiner () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void WebMiner::Miner_reset () {
}

//----------------------------------------------------------------//
void WebMiner::Miner_shutdown ( bool kill ) {

    if ( !this->isStopped ()) {
    
        this->Miner::Miner_shutdown ( kill );
        this->stop ();
        
        if ( kill ) {
            printf ( "REQUESTED WEB MINER SHUTDOWN\n" );
            this->mShutdownEvent.set ();
            Poco::Util::ServerApplication::terminate ();
        }
    }
    
    if ( !kill ) {
        this->wait ();
    }
}

} // namespace Volition
