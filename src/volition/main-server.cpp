// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <padamose/padamose.h>
#include <volition/Block.h>
#include <volition/FileSys.h>
#include <volition/RouteTable.h>
#include <volition/Singleton.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/version.h>
#include <volition/MinerActivity.h>
#include <volition/MinerAPIFactory.h>

using namespace Volition;

//================================================================//
// ServerApp
//================================================================//
class ServerApp :
public Poco::Util::ServerApplication {
protected:

    shared_ptr < MinerActivity > mMinerActivity;

    //----------------------------------------------------------------//
    void defineOptions ( Poco::Util::OptionSet& options ) override {
        Application::defineOptions ( options );
        
        options.addOption (
            Poco::Util::Option ( "config", "c", "path to configuration file" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "config" )
        );
        
        options.addOption (
            Poco::Util::Option ( "control-key", "", "path to public key for verifying control commands." )
                .required ( false )
                .argument ( "value", true )
                .binding ( "control-key" )
        );
        
        options.addOption (
            Poco::Util::Option ( "control-level", "", "miner control level ('config', 'admin')" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "control-level" )
        );
        
        options.addOption (
            Poco::Util::Option ( "delay-fixed", "df", "set update interval (in milliseconds)" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "delay-fixed" )
        );
        
        options.addOption (
            Poco::Util::Option ( "delay-variable", "dv", "set update interval (in milliseconds)" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "delay-variable" )
        );
        
        options.addOption (
            Poco::Util::Option ( "dump", "dmp", "dump ledger to sqlite file" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "dump" )
        );
        
        options.addOption (
            Poco::Util::Option ( "genesis", "g", "path to the genesis file" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "genesis" )
        );
        
        options.addOption (
            Poco::Util::Option ( "keyfile", "k", "path to public miner key file" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "keyfile" )
        );
        
        options.addOption (
            Poco::Util::Option ( "logpath", "l", "path to log folder" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "logpath" )
        );
        
        options.addOption (
            Poco::Util::Option ( "miner", "m", "miner name" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "miner" )
        );
        
        options.addOption (
            Poco::Util::Option ( "persist", "", "path to folder for persist files." )
                .required ( false )
                .argument ( "value", true )
                .binding ( "persist" )
        );
        
        options.addOption (
            Poco::Util::Option ( "port", "p", "set port to serve from" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "port" )
        );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string >& ) override {
    
        #ifdef NDEBUG
            LGN_LOG ( VOL_FILTER_APP, INFO, "NDEBUG" );
        #else
            LGN_LOG ( VOL_FILTER_APP, INFO, "NDEBUG NOT DEFINED" );
        #endif

        #ifdef _DEBUG
            LGN_LOG ( VOL_FILTER_APP, INFO, "_DEBUG" );
        #else
            LGN_LOG ( VOL_FILTER_APP, INFO, "_DEBUG NOT DEFINED" );
        #endif
    
        Poco::Util::AbstractConfiguration& configuration = this->config ();
        
        string configfile = configuration.getString ( "config", "" );
        
        if ( configfile.size () > 0 ) {
            this->loadConfiguration ( configfile, PRIO_APPLICATION - 1 );
        }
        else {
            this->loadConfiguration ( PRIO_APPLICATION - 1 );
        }
        
        this->printProperties ();
        
        string controlKeyfile           = configuration.getString       ( "control-key", "" );
        string controlLevel             = configuration.getString       ( "control-level", "" );
        string dump                     = configuration.getString       ( "dump", "" );
        string genesis                  = configuration.getString       ( "genesis", "genesis.json" );
        int fixedDelay                  = configuration.getInt          ( "delay-fixed", MinerActivity::DEFAULT_FIXED_UPDATE_MILLIS );
        int variableDelay               = configuration.getInt          ( "delay-variable", MinerActivity::DEFAULT_VARIABLE_UPDATE_MILLIS );
        string keyfile                  = configuration.getString       ( "keyfile" );
        string logpath                  = configuration.getString       ( "logpath", "" );
        string minerID                  = configuration.getString       ( "miner", "" );
        string nodelist                 = configuration.getString       ( "nodelist", "" );
        string persist                  = configuration.getString       ( "persist", "persist-chain" );
        int port                        = configuration.getInt          ( "port", 9090 );
        string sslCertFile              = configuration.getString       ( "openSSL.server.certificateFile", "" );
        
        if ( logpath.size () > 0 ) {
                    
            time_t t;
            time ( &t );
            string timeStr = Poco::DateTimeFormatter ().format ( Poco::Timestamp ().fromEpochTime ( t ), "%Y-%m-%d-%H%M%S" );
            string logname = Format::write ( "%s/%s-%s.log", logpath.c_str (), minerID.c_str (), timeStr.c_str ());
            string lognameErr = Format::write ( "%s/%s-%s-error.log", logpath.c_str (), minerID.c_str (), timeStr.c_str ());
            
            loguru::add_file ( logname.c_str (), loguru::Append, loguru::Verbosity_MAX );
            loguru::add_file ( lognameErr.c_str (), loguru::Append, loguru::Verbosity_WARNING );
        }
        
        LGN_LOG ( VOL_FILTER_APP, INFO, "Hello from VOLITION main.cpp!" );
        LGN_LOG ( VOL_FILTER_APP, INFO, "commit: %s", VOLITION_GIT_COMMIT_STR );
        LGN_LOG ( VOL_FILTER_APP, INFO, "build: %s %s", VOLITION_BUILD_DATE_STR, VOLITION_GIT_TAG_STR );
        
        this->mMinerActivity = make_shared < MinerActivity >();
        this->mMinerActivity->setMinerID ( minerID );
        
        if ( controlKeyfile.size ()) {
        
            CryptoPublicKey controlKey;
            controlKey.load ( controlKeyfile );
            
            if ( !controlKey ) {
                LGN_LOG ( VOL_FILTER_APP, INFO, "CONTROL KEY NOT FOUND: %s", controlKeyfile.c_str ());
                return Application::EXIT_CONFIG;
            }
            LGN_LOG ( VOL_FILTER_APP, INFO, "CONTROL KEY: %s", controlKeyfile.c_str ());
            this->mMinerActivity->setControlKey ( controlKey );
        }
        
        switch ( FNV1a::hash_64 ( controlLevel )) {
        
            case FNV1a::const_hash_64 ( "config" ):
                LGN_LOG ( VOL_FILTER_APP, INFO, "CONTROL LEVEL: CONFIG" );
                this->mMinerActivity->setControlLevel ( Miner::CONTROL_CONFIG );
                break;
            
            case FNV1a::const_hash_64 ( "admin" ):
               LGN_LOG ( VOL_FILTER_APP, INFO, "CONTROL LEVEL: ADMIN" );
                this->mMinerActivity->setControlLevel ( Miner::CONTROL_ADMIN );
                break;
        }
        
        LGN_LOG ( VOL_FILTER_APP, INFO, "LOADING GENESIS BLOCK: %s", genesis.c_str ());
        if ( !FileSys::exists ( genesis )) {
            LGN_LOG ( VOL_FILTER_APP, INFO, "...BUT THE FILE DOES NOT EXIST!" );
            return Application::EXIT_CONFIG;
        }
        
        shared_ptr < Block > genesisBlock = Miner::loadGenesisBlock ( genesis );
        
        if ( !genesisBlock ) {
            LGN_LOG ( VOL_FILTER_APP, INFO, "UNABLE TO LOAD GENESIS BLOCK" );
            return Application::EXIT_CONFIG;
        }
        
        if ( persist.size ()) {
            LedgerResult result = this->mMinerActivity->persist ( persist, genesisBlock );
            if ( !result ) {
                LGN_LOG ( VOL_FILTER_APP, INFO, "ERROR LOADING OR INITIALIZING PERSISTENCE" );
                LGN_LOG ( VOL_FILTER_APP, INFO, "%s", result.getMessage ().c_str ());
                return Application::EXIT_CONFIG;
            }
        }
        else {
            this->mMinerActivity->setGenesis ( genesisBlock );
        }
        
        if ( this->mMinerActivity->getLedger ().countBlocks () == 0 ) {
            LGN_LOG ( VOL_FILTER_APP, INFO, "MISSING OR CORRUPT GENESIS BLOCK" );
            return Application::EXIT_CONFIG;
        }
        
        if ( dump.size ()) {
            this->mMinerActivity->getLedger ().dump ( dump );
            return Application::EXIT_OK;
        }
        
        LGN_LOG ( VOL_FILTER_APP, INFO, "LOADING KEY FILE: %s", keyfile.c_str ());
        if ( !FileSys::exists ( keyfile )) {
            LGN_LOG ( VOL_FILTER_APP, INFO, "...BUT THE FILE DOES NOT EXIST!" );
            return Application::EXIT_CONFIG;
        }
        
        this->mMinerActivity->loadKey ( keyfile );
        this->mMinerActivity->affirmKey ();
        this->mMinerActivity->affirmVisage ();
        this->mMinerActivity->setVerbose ();
        this->mMinerActivity->setReportMode ( Miner::REPORT_ALL_BRANCHES );
        this->mMinerActivity->setFixedUpdateDelayInMillis (( u32 )fixedDelay );
        this->mMinerActivity->setVariableUpdateDelayInMillis (( u32 )variableDelay );
        
        LGN_LOG ( VOL_FILTER_APP, INFO, "MINER ID: %s", this->mMinerActivity->getMinerID ().c_str ());

        this->serve ( port, sslCertFile.length () > 0 );
        
        LGN_LOG ( VOL_FILTER_APP, INFO, "SHUTDOWN: main" );
        
        return Application::EXIT_OK;
    }
    
    //----------------------------------------------------------------//
    const char* name () const override {
    
        return "volition";
    }
    
    //----------------------------------------------------------------//
    void printProperties ( const std::string& base = "" ) {
    
        Poco::Util::AbstractConfiguration::Keys keys;
        this->config ().keys ( base, keys );
    
        if ( keys.empty ()) {
            if ( config ().hasProperty ( base )) {
                std::string msg;
                msg.append ( base );
                msg.append ( " = " );
                msg.append ( config ().getString ( base ));
                LGN_LOG ( VOL_FILTER_APP, INFO, "%s", msg.c_str ())
            }
        }
        else {
            for ( Poco::Util::AbstractConfiguration::Keys::const_iterator it = keys.begin (); it != keys.end (); ++it ) {
                std::string fullKey = base;
                if ( !fullKey.empty ()) fullKey += '.';
                fullKey.append ( *it );
                printProperties ( fullKey );
            }
        }
    }
    
    //----------------------------------------------------------------//
    void serve ( int port, bool ssl ) {

        Poco::ThreadPool threadPool;

        Poco::Net::HTTPServer server (
            new MinerAPIFactory ( this->mMinerActivity ),
            threadPool,
            ssl ? Poco::Net::SecureServerSocket (( Poco::UInt16 )port ) : Poco::Net::ServerSocket (( Poco::UInt16 )port ),
            new Poco::Net::HTTPServerParams ()
        );
        
        server.start ();
        this->mMinerActivity->start ();

        LGN_LOG ( VOL_FILTER_APP, INFO, "SERVING YOU BLOCKCHAIN REALNESS ON PORT: %d", port );
        this->mMinerActivity->waitForShutdown ();
        LGN_LOG ( VOL_FILTER_APP, INFO, "SHUTDOWN: this->mMinerActivity->waitForShutdown ()" );

        server.stop ();
        threadPool.stopAll ();
        
        {
            // wait for miner activity to fully shut down
            ScopedExclusiveMinerLock scopedLock ( this->mMinerActivity );
            this->mMinerActivity->shutdown ( false );
        }
        LGN_LOG ( VOL_FILTER_APP, INFO, "SHUTDOWN: ~serve" );
    }

public:

    //----------------------------------------------------------------//
    ServerApp () {
    
        Poco::Net::initializeSSL ();
    }
    
    //----------------------------------------------------------------//
    ~ServerApp () {
    
        Poco::Net::uninitializeSSL ();
        LGN_LOG ( VOL_FILTER_APP, INFO, "SHUTDOWN: ~ServerApp" );
    }
    
    //----------------------------------------------------------------//
    void shutdown () {
    
        this->mMinerActivity->shutdown ( true );
    }
};

//================================================================//
// main
//================================================================//

static shared_ptr < ServerApp > sApp;

//----------------------------------------------------------------//
void onSignal ( int sig );
void onSignal ( int sig ) {

    signal ( sig, SIG_IGN );

    LGN_LOG ( VOL_FILTER_APP, INFO, "CAUGHT A SIGNAL - SHUTTING DOWN." );

    if ( sApp ) {
        sApp->shutdown ();
    }
}

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

    signal ( SIGINT, onSignal );
    signal ( SIGTERM, onSignal );
    signal ( SIGQUIT, onSignal );

    // force line buffering even when running as a spawned process
    setvbuf ( stdout, NULL, _IOLBF, 0 );
    setvbuf ( stderr, NULL, _IOLBF, 0 );

    Lognosis::setFilter ( PDM_FILTER_ROOT,                      Lognosis::WARNING );
    Lognosis::setFilter ( PDM_FILTER_SQLITE,                    Lognosis::WARNING );
    Lognosis::setFilter ( PDM_FILTER_SQLSTORE,                  Lognosis::WARNING );
    
//    Lognosis::setFilter ( VOL_FILTER_APP,                       Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_BLOCK,                     Lognosis::OFF );
//    Lognosis::setFilter ( VOL_FILTER_CONSENSUS,                 Lognosis::OFF );
    
    Lognosis::setFilter ( VOL_FILTER_HTTP,                      Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_JSON,                      Lognosis::OFF );
    Lognosis::setFilter ( VOL_FILTER_LEDGER,                    Lognosis::OFF );
//    Lognosis::setFilter ( VOL_FILTER_LUA,                       Lognosis::OFF );
    
//    Lognosis::setFilter ( VOL_FILTER_MINING_REPORT,             Lognosis::OFF );
//    Lognosis::setFilter ( VOL_FILTER_MINING_SEARCH_REPORT,      Lognosis::OFF );
//    Lognosis::setFilter ( VOL_FILTER_QUEUE,                     Lognosis::OFF );
    
    Lognosis::init ( argc, argv );

    sApp = make_shared < ServerApp >();
    int result = sApp->run ( argc, argv );
    sApp = NULL;
    return result;
}
