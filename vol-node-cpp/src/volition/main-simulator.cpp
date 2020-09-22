// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <padamose/padamose.h>
#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/FileSys.h>
#include <volition/Miner.h>
#include <volition/MinerAPIFactory.h>
#include <volition/RouteTable.h>
#include <volition/simulation/Analysis.h>
#include <volition/SimMiningMessenger.h>
#include <volition/Singleton.h>
#include <volition/TheContext.h>
#include <volition/transactions/Genesis.h>
#include <volition/version.h>

using namespace Volition;

const int BASE_PORT         = 9090;
const size_t TOTAL_MINERS   = 16;

//================================================================//
// Monitor
//================================================================//
class Monitor :
    public Poco::Activity < Monitor > {
private:
    
    friend class SimulatorApp;
    
    vector < shared_ptr < Miner >>&     mMiners;
    shared_ptr < SimMiningMessenger >   mMessenger;
    Poco::Event                         mShutdownEvent;
    
    BlockTree                           mOptimal;
    BlockTreeNode::ConstPtr             mOptimalTag;

    //----------------------------------------------------------------//
    void extendOptimal ( size_t height ) {
    
        BlockTreeNode::ConstPtr tail = this->mOptimalTag;
        assert ( tail );
        
        while (( **tail ).getHeight () < height ) {
                    
            shared_ptr < const Block > parent = tail->getBlock ();
            
            shared_ptr < Miner > bestMiner;
            Digest bestCharm;
            
            for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
                shared_ptr < Miner > miner = this->mMiners [ i ];
                Digest charm = parent->getNextCharm ( miner->getVisage ());
                                
                if ( !bestMiner || ( BlockHeader::compare ( charm, bestCharm ) < 0 )) {
                    bestMiner = miner;
                    bestCharm = charm;
                }
            }
            
            shared_ptr < Block > child = make_shared < Block >(
                bestMiner->getMinerID (),
                bestMiner->getVisage (),
                0,
                parent.get (),
                bestMiner->getKeyPair ()
            );
            child->sign ( bestMiner->getKeyPair ());

            tail = this->mOptimal.affirmBlock ( child );
            this->mOptimalTag = tail;
        }
    }

    //----------------------------------------------------------------//
    void runActivity () {
    
        Simulation::Analysis analysis;
    
        while ( !this->isStopped ()) {
            
            Simulation::Tree tree;
            
            for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
                shared_ptr < Miner > miner = this->mMiners [ i ];
                miner->step ();
                ScopedMinerLock minerLock ( miner );
                tree.addChain ( *this->mMiners [ i ]->getBestBranch ());
            }
            
            this->mMessenger->updateAndDispatch ();
            
//            shared_ptr < const BlockTreeNode > tail = this->mMiners [ 0 ]->getBlockTreeTag ();
//            LGN_LOG ( VOL_FILTER_ROOT, INFO, "9090: %s", tail->writeBranch ().c_str ());
//            
//            this->extendOptimal (( **tail ).getHeight ());
//            LGN_LOG ( VOL_FILTER_ROOT, INFO, "GOAL: %s", this->mOptimalTag->writeBranch ().c_str ());
//            
//            LGN_LOG ( VOL_FILTER_ROOT, INFO, "" );
            
            analysis.update ( tree );
            analysis.log ( "", true, 1 );
            
            Poco::Thread::sleep ( 10 );
        }
    }

public:

    //----------------------------------------------------------------//
    Monitor ( vector < shared_ptr < Miner >>& miners, shared_ptr < SimMiningMessenger > messenger ) :
        Poco::Activity < Monitor >( this, &Monitor::runActivity ),
        mMiners ( miners ),
        mMessenger ( messenger ) {
    }
    
    //----------------------------------------------------------------//
    ~Monitor () {
    }
    
    //----------------------------------------------------------------//
    void shutdown () {
    
        if ( !this->isStopped ()) {
            this->stop ();
        }
        this->wait ();
    }
};

//================================================================//
// SimulatorApp
//================================================================//
class SimulatorApp :
    public Poco::Util::ServerApplication {
public:

    //----------------------------------------------------------------//
    int main ( const vector < string > &args ) override {
        UNUSED ( args );

        // TODO: factor all this setup into Monitor object (and rename 'Monitor' to 'Sim')

        vector < shared_ptr < Miner >> miners;
        miners.resize ( TOTAL_MINERS );

        shared_ptr < Transactions::Genesis > genesisMinerTransactionBody = make_unique < Transactions::Genesis >();
        genesisMinerTransactionBody->setIdentity ( "SIMULATION" );

        shared_ptr < SimMiningMessenger > messenger = make_shared < SimMiningMessenger >();

        for ( size_t i = 0; i < TOTAL_MINERS; ++i ) {
        
            shared_ptr < Miner > miner = make_shared < Miner >();
            miners [ i ] = miner;

            miner->setMinerID ( Format::write ( "%d", BASE_PORT + ( int )i ));
            miner->affirmKey ();
            miner->affirmVisage ();
            miner->setMessenger ( messenger );

            Transactions::GenesisAccount genesisAccount;
            
            genesisAccount.mName    = miner->getMinerID ();
            genesisAccount.mKey     = miner->getKeyPair ();
            genesisAccount.mGrant   = 0;
            genesisAccount.mURL     = Format::write ( "http://127.0.0.1:%s/%s/", Format::write ( "%d", BASE_PORT ).c_str (), miner->getMinerID ().c_str ());

            genesisAccount.mMotto   = miner->getMotto ();
            genesisAccount.mVisage  = miner->getVisage ();

            genesisMinerTransactionBody->pushAccount ( genesisAccount );
        }
        messenger->setMiners ( miners );

        shared_ptr < Transaction > transaction = make_shared < Transaction >();
        transaction->setBody ( move ( genesisMinerTransactionBody ));
        
        shared_ptr < Block > genesisBlock = make_shared < Block >();
        genesisBlock->pushTransaction ( transaction );
        genesisBlock->affirmHash ();

        for ( size_t i = 0; i < TOTAL_MINERS; ++i ) {
            miners [ i ]->setGenesis ( genesisBlock );
        }
        
        Poco::ThreadPool threadPool;
        
        shared_ptr < Poco::Net::HTTPServer > server = make_shared < Poco::Net::HTTPServer >(
            new Volition::MinerAPIFactory ( miners ),
            threadPool,
            Poco::Net::ServerSocket (( Poco::UInt16 )BASE_PORT ),
            new Poco::Net::HTTPServerParams ()
        );
        server->start ();

        Monitor monitor ( miners, messenger );
        monitor.mOptimalTag = monitor.mOptimal.affirmBlock ( genesisBlock );
        monitor.start ();

        // nasty little hack. POCO considers the set breakpoint signal to be a termination event.
        // need to find out how to stop POCO from doing this. in the meantime, this hack.
        #ifdef _DEBUG
            Poco::Event shutdownEvent;
            shutdownEvent.wait ();
        #else
            this->waitForTerminationRequest ();  // wait for CTRL-C or kill
        #endif

        monitor.shutdown ();

        server->stop ();
        
        threadPool.stopAll ();

        return EXIT_OK;
    }
};

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

    Lognosis::setFilter ( PDM_FILTER_ROOT, Lognosis::OFF );
    Lognosis::init ( argc, argv );
    LOG_F ( INFO, "Hello from main.cpp!" );

    SimulatorApp app;
    return app.run ( argc, argv );
}
