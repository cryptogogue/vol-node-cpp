// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <padamose/padamose.h>
#include <volition/Block.h>
#include <volition/FileSys.h>
#include <volition/RouteTable.h>
#include <volition/simulation/Analysis.h>
#include <volition/Singleton.h>
#include <volition/TheContext.h>
#include <volition/transactions/Genesis.h>
#include <volition/version.h>
#include <volition/WebMiner.h>
#include <volition/WebMinerAPIFactory.h>

using namespace Volition;

const int BASE_PORT         = 9090;
const size_t TOTAL_MINERS   = 16;

//================================================================//
// Monitor
//================================================================//
class Monitor :
    public Poco::Activity < Monitor > {
private:
    
    vector < shared_ptr < WebMiner >>&  mMiners;
    Poco::Event                         mShutdownEvent;

    //----------------------------------------------------------------//
    void runActivity () {
    
        Simulation::Analysis analysis;
    
        while ( !this->isStopped ()) {
            
            Simulation::Tree tree;
            
            for ( size_t i = 0; i < this->mMiners.size (); ++i ) {
                shared_ptr < WebMiner > miner = this->mMiners [ i ];
                miner->step ();
                ScopedWebMinerLock minerLock ( miner );
                tree.addChain ( *this->mMiners [ i ]->getBestBranch ());
            }
            
            //this->mMiners [ 0 ]->getBlockTree ().logTree ( "9090: ", 1 );
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "9090: %s", this->mMiners [ 0 ]->getBlockTreeTag ().getNode ()->writeBranch ().c_str ());
            
//            analysis.update ( tree );
//            analysis.log ( "", true, 1 );
            
            Poco::Thread::sleep ( 10 );
        }
    }

public:

    //----------------------------------------------------------------//
    Monitor ( vector < shared_ptr < WebMiner >>& miners ) :
        Poco::Activity < Monitor >( this, &Monitor::runActivity ),
        mMiners ( miners ) {
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
    
        vector < shared_ptr < WebMiner >> miners;
        miners.resize ( TOTAL_MINERS );

        shared_ptr < Transactions::Genesis > genesisMinerTransactionBody = make_unique < Transactions::Genesis >();
        genesisMinerTransactionBody->setIdentity ( "SIMULATION" );

        for ( size_t i = 0; i < TOTAL_MINERS; ++i ) {
        
            shared_ptr < WebMiner > miner = make_shared < WebMiner >();
            miners [ i ] = miner;

            miner->setMinerID ( Format::write ( "%d", BASE_PORT + ( int )i ));
            miner->affirmKey ();

            Transactions::GenesisAccount genesisAccount;
            
            genesisAccount.mName    = miner->getMinerID ();
            genesisAccount.mKey     = miner->getKeyPair ();
            genesisAccount.mGrant   = 0;
            genesisAccount.mURL     = Format::write ( "http://127.0.0.1:%s/%s/", Format::write ( "%d", BASE_PORT ).c_str (), miner->getMinerID ().c_str ());

            genesisMinerTransactionBody->pushAccount ( genesisAccount );
        }

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
            new Volition::WebMinerAPIFactory ( miners ),
            threadPool,
            Poco::Net::ServerSocket (( Poco::UInt16 )BASE_PORT ),
            new Poco::Net::HTTPServerParams ()
        );
        server->start ();

        Monitor monitor ( miners );
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
