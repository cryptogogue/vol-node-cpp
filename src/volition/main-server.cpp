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
    void addOption ( Poco::Util::OptionSet& opts, string name, string shortname, string desc, string oneOf = "", string def = "" ) {
    
        string oneOfStr     = oneOf.size () ? Format::write ( "one of: %s.", oneOf.c_str ()) : "";
        string defStr       = def.size () ? Format::write ( "default: %s.", def.c_str ()) : "";
    
        opts.addOption (
            Poco::Util::Option ( name, shortname, Format::write ( "%s. %s %s", desc.c_str (), oneOfStr.c_str (), defStr.c_str ()))
                .required ( false )
                .argument ( "value", true )
                .binding ( name )
        );
    }

    //----------------------------------------------------------------//
    void defineOptions ( Poco::Util::OptionSet& opts ) override {
        Application::defineOptions ( opts );
        
        // log levels
        this->addOption ( opts, "log-level-app", "",                    "log level for the application",                                "info, warn, error, fatal",     "info" );
        this->addOption ( opts, "log-level-block", "",                  "log level for push block internals",                           "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-consensus", "",              "log level for consensus",                                      "info, warn, error, fatal",     "info" );
        this->addOption ( opts, "log-level-http", "",                   "log level for http handlers",                                  "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-json", "",                   "log level for json serialization",                             "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-ledger", "",                 "log level for ledger operations",                              "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-lua", "",                    "log level for lua (crafting) scripts",                         "info, warn, error, fatal",     "info" );
        this->addOption ( opts, "log-level-mining-report", "",          "log level for the mining report",                              "info, warn, error, fatal",     "info" );
        this->addOption ( opts, "log-level-padamose", "",               "log level for the padamose versioned store",                   "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-padamose-lock", "",          "log level for padamose tree locking",                          "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-padamose-optimize", "",      "log level for padamose tree optimization",                     "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-padamose-sqlite", "",        "log level for the sqlite persistence provider in padamose",    "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-search-report", "",          "log level for the block search report",                        "info, warn, error, fatal",     "info" );
        this->addOption ( opts, "log-level-sqlite", "",                 "log level for sqlite",                                         "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-store", "",                  "log level for persistent storage",                             "info, warn, error, fatal",     "warn" );
        this->addOption ( opts, "log-level-transaction-queue", "",      "log level for transaction queue",                              "info, warn, error, fatal",     "warn" );
        
        // general config
        this->addOption ( opts, "block-search-max", "",                 "maximum number of simultaneous block searches",                            "",                     "256" );
        this->addOption ( opts, "blocktree-persist-mode", "",           "the persist mode",                                                         "none, sqlite",         "sqlite" );
        this->addOption ( opts, "config", "c",                          "path to configuration file" );
        this->addOption ( opts, "control-key", "",                      "path to public key for verifying control commands" );
        this->addOption ( opts, "control-level", "",                    "miner control level",                                                      "none, config, admin",  "none" );
        this->addOption ( opts, "dump", "",                             "dump ledger to sqlite give filename" );
        this->addOption ( opts, "genesis", "g",                         "path to the genesis file",                                                 "",                     "genesis.json" );
        this->addOption ( opts, "keyfile", "k",                         "path to public miner key file" );
        this->addOption ( opts, "ledger-persist-check-retry", "",       "retry the post-save integrity check N times",                              "",                     "0" );
        this->addOption ( opts, "ledger-persist-frequency", "",         "force a persist every N blocks (during chain composition)",                "",                     "0" );
        this->addOption ( opts, "ledger-persist-mode", "",              "the persist mode",                                                         "none, sqlite, sqlite-stringstore, debug-stringstore",        "sqlite" );
        this->addOption ( opts, "ledger-persist-sleep", "",             "sleep N milliseconds after ledger persist",                                "",                     "0" );
        this->addOption ( opts, "logpath", "l",                         "path to log folder" );
        this->addOption ( opts, "miner", "m",                           "mining account name" );
        this->addOption ( opts, "persist", "",                          "alias for 'persist-path'; DEPRECATED",                                     "",                     "persist" );
        this->addOption ( opts, "persist-path", "",                     "base path to folder for persist files",                                    "",                     "persist-chain" );
        this->addOption ( opts, "port", "p",                            "set port to serve from",                                                   "",                     "9090" );
        this->addOption ( opts, "sleep-fixed", "",                      "set fixed update sleep (in milliseconds)"                                  "",                     "1000" );
        this->addOption ( opts, "sleep-variable", "",                   "set variable update sleep (in milliseconds)"                               "",                     "1000" );
        this->addOption ( opts, "sqlite-journal-mode", "",              "the sqlite journaling mode",                                               "rollback, wal",        "wal" );
//        this->addOption ( opts, "sqlite-sleep-frequency", "",           "sleep after N writes",                                                     "",                     "0" );
//        this->addOption ( opts, "sqlite-sleep-millis", "",              "approx milliseconds to sleep if 'sqlite-sleep-frequency' is non-zero",     "",                     "100" );
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
        
        // log levels
        Lognosis::setFilter ( VOL_FILTER_APP,                       this->stringToLogLevel ( configuration.getString ( "log-level-app",                     "info" )));
        Lognosis::setFilter ( VOL_FILTER_BLOCK,                     this->stringToLogLevel ( configuration.getString ( "log-level-block",                   "warn" )));
        Lognosis::setFilter ( VOL_FILTER_CONSENSUS,                 this->stringToLogLevel ( configuration.getString ( "log-level-consensus",               "info" )));
        Lognosis::setFilter ( VOL_FILTER_HTTP,                      this->stringToLogLevel ( configuration.getString ( "log-level-http",                    "warn" )));
        Lognosis::setFilter ( VOL_FILTER_JSON,                      this->stringToLogLevel ( configuration.getString ( "log-level-json",                    "warn" )));
        Lognosis::setFilter ( VOL_FILTER_LEDGER,                    this->stringToLogLevel ( configuration.getString ( "log-level-ledger",                  "warn" )));
        Lognosis::setFilter ( VOL_FILTER_LUA,                       this->stringToLogLevel ( configuration.getString ( "log-level-lua",                     "info" )));
        Lognosis::setFilter ( VOL_FILTER_MINING_REPORT,             this->stringToLogLevel ( configuration.getString ( "log-level-mining-report",           "info" )));
        Lognosis::setFilter ( PDM_FILTER_ROOT,                      this->stringToLogLevel ( configuration.getString ( "log-level-padamose",                "warn" )));
        Lognosis::setFilter ( PDM_FILTER_LOCK,                      this->stringToLogLevel ( configuration.getString ( "log-level-padamose-lock",           "warn" )));
        Lognosis::setFilter ( PDM_FILTER_OPTIMIZE,                  this->stringToLogLevel ( configuration.getString ( "log-level-padamose-optimize",       "warn" )));
        Lognosis::setFilter ( PDM_FILTER_SQLSTORE,                  this->stringToLogLevel ( configuration.getString ( "log-level-padamose-sqlite",         "warn" )));
        Lognosis::setFilter ( VOL_FILTER_MINING_SEARCH_REPORT,      this->stringToLogLevel ( configuration.getString ( "log-level-search-report",           "info" )));
        Lognosis::setFilter ( PDM_FILTER_SQLITE,                    this->stringToLogLevel ( configuration.getString ( "log-level-sqlite",                  "warn" )));
        Lognosis::setFilter ( VOL_FILTER_STORE,                     this->stringToLogLevel ( configuration.getString ( "log-level-store",                   "warn" )));
        Lognosis::setFilter ( VOL_FILTER_TRANSACTION_QUEUE,         this->stringToLogLevel ( configuration.getString ( "log-level-transaction-queue",       "warn" )));
        
        Lognosis::useCurrentFilterForAllNewThreads ();
        
        // general config
        string blockTreePersistMode         = configuration.getString       ( "blocktree-persist-mode", "sqlite" );
        int blockSearchMax                  = configuration.getInt          ( "block-search-max", 256 );
        int blockTreeCacheSize              = configuration.getInt          ( "block-tree-cache-size", Miner::DEFAULT_BLOCK_TREE_CACHE_SIZE );
        int consensusLookaheadHeight        = configuration.getInt          ( "consensus-lookahead-height", Miner::DEFAULT_CONSENSUS_LOOKAHEAD_HEIGHT );
        string controlKeyfile               = configuration.getString       ( "control-key", "" );
        string controlLevel                 = configuration.getString       ( "control-level", "" );
        string dump                         = configuration.getString       ( "dump", "" );
        string genesis                      = configuration.getString       ( "genesis", "genesis.json" );
        string keyfile                      = configuration.getString       ( "keyfile" );
        int ledgerPersistCheckRetry         = configuration.getInt          ( "ledger-persist-check-retry", 0 );
        int ledgerPersistFrequency          = configuration.getInt          ( "ledger-persist-frequency", 0 );
        string ledgerPersistMode            = configuration.getString       ( "ledger-persist-mode", "sqlite" );
        int ledgerPersistSleep              = configuration.getInt          ( "ledger-persist-sleep", 0 );
        string logpath                      = configuration.getString       ( "logpath", "" );
        string minerID                      = configuration.getString       ( "miner", "" );
        string nodelist                     = configuration.getString       ( "nodelist", "" );
        string persistPath                  = configuration.getString       ( "persist", "persist-chain" );
        persistPath                         = configuration.getString       ( "persist-path", persistPath );
        int port                            = configuration.getInt          ( "port", 9090 );
        int sleepFixed                      = configuration.getInt          ( "sleep-fixed", MinerActivity::DEFAULT_FIXED_UPDATE_MILLIS );
        int sleepVariable                   = configuration.getInt          ( "sleep-variable", MinerActivity::DEFAULT_VARIABLE_UPDATE_MILLIS );
        string sqliteJournalMode            = configuration.getString       ( "sqlite-journal-mode", "wal" );
//        int sqliteSleepFrequency            = configuration.getInt          ( "sqlite-sleep-frequency", 0 ); // TODO
//        int sqliteSleepMillis               = configuration.getInt          ( "sqlite-sleep-millis", 100 ); // TODO
        string sslCertFile                  = configuration.getString       ( "openSSL.server.certificateFile", "" );
        
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
        
        this->mMinerActivity->setPersistFrequency (( size_t )ledgerPersistFrequency );
        this->mMinerActivity->setRetryPersistenceCheck (( size_t )ledgerPersistCheckRetry );
        this->mMinerActivity->setPersistenceSleep (( size_t )ledgerPersistSleep );
        
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
            LGN_LOG ( VOL_FILTER_APP, ERROR, "...BUT THE FILE DOES NOT EXIST!" );
            return Application::EXIT_CONFIG;
        }
        
        shared_ptr < Block > genesisBlock = Miner::loadGenesisBlock ( genesis );
        
        if ( !genesisBlock ) {
            LGN_LOG ( VOL_FILTER_APP, ERROR, "UNABLE TO LOAD GENESIS BLOCK" );
            return Application::EXIT_CONFIG;
        }
        
        if ( persistPath.size ()) {
            this->mMinerActivity->setPersistencePath ( persistPath, genesisBlock );
        }
        
        SQLiteConfig sqliteConfig;
        
        sqliteConfig.mJournalMode = stringToJournalMode ( sqliteJournalMode );
        if ( sqliteConfig.mJournalMode == SQLiteConfig::JOURNAL_MODE_UNKNOWN ) {
            LGN_LOG ( VOL_FILTER_APP, ERROR, "UNRECOGNIZED SQLITE JOURNAL MODE %s", sqliteJournalMode.c_str ());
            return Application::EXIT_CONFIG;
        }
        
        bool inMemoryBlockTree = false;
        bool inMemoryLedger = false;
        
        {
            LedgerResult result = true;
            
            switch ( FNV1a::hash_64 ( blockTreePersistMode.c_str ())) {
                case FNV1a::const_hash_64 ( "none" ): {
                    this->mMinerActivity->setBlockTree ();
                    inMemoryBlockTree = true;
                    break;
                }
                case FNV1a::const_hash_64 ( "sqlite" ): {
                    result = this->mMinerActivity->persistBlockTreeSQLite ( sqliteConfig );
                    break;
                }
                default: {
                    LGN_LOG ( VOL_FILTER_APP, ERROR, "UNRECOGNIZED BLOCK TREE PERSISTENCE MODE: %s", blockTreePersistMode.c_str ());
                    return Application::EXIT_CONFIG;
                }
            }
            
            if ( !result ) {
                LGN_LOG ( VOL_FILTER_APP, ERROR, "ERROR LOADING OR INITIALIZING BLOCK TREE PERSISTENCE" );
                LGN_LOG ( VOL_FILTER_APP, ERROR, "%s", result.getMessage ().c_str ());
                return Application::EXIT_CONFIG;
            }
        }
        
        {
            LedgerResult result = true;
        
            switch ( FNV1a::hash_64 ( ledgerPersistMode.c_str ())) {
                case FNV1a::const_hash_64 ( "none" ): {
                    this->mMinerActivity->setGenesis ( genesisBlock );
                    inMemoryLedger = true;
                    break;
                }
                case FNV1a::const_hash_64 ( "sqlite" ): {
                    result = this->mMinerActivity->persistLedgerSQLite ( genesisBlock, sqliteConfig );
                    break;
                }
                case FNV1a::const_hash_64 ( "sqlite-stringstore" ): {
                    result = this->mMinerActivity->persistLedgerSQLiteStringStore ( genesisBlock, sqliteConfig );
                    break;
                }
                case FNV1a::const_hash_64 ( "debug-stringstore" ): {
                    result = this->mMinerActivity->persistLedgerDebugStringStore ( genesisBlock );
                    break;
                }
                default: {
                    LGN_LOG ( VOL_FILTER_APP, ERROR, "UNRECOGNIZED LEDGER PERSISTENCE MODE: %s", ledgerPersistMode.c_str ());
                    return Application::EXIT_CONFIG;
                }
            }
            
            if ( !result ) {
                LGN_LOG ( VOL_FILTER_APP, ERROR, "ERROR LOADING OR INITIALIZING LEDGER PERSISTENCE" );
                LGN_LOG ( VOL_FILTER_APP, ERROR, "%s", result.getMessage ().c_str ());
                return Application::EXIT_CONFIG;
            }
        }
        
        if ( inMemoryBlockTree && !inMemoryLedger ) {
            LGN_LOG ( VOL_FILTER_APP, ERROR, "IN-MEMORY BLOCK TREE SHOULD NOT BE MIXED WITH PERSISTENT LEDGER" );
            LGN_LOG ( VOL_FILTER_APP, ERROR, "PLEASE DELETE PERSISTENCE FILES AND TRY AGAIN" );
            return Application::EXIT_CONFIG;
        }
        
        if ( this->mMinerActivity->getLedger ().countBlocks () == 0 ) {
            LGN_LOG ( VOL_FILTER_APP, ERROR, "MISSING OR CORRUPT GENESIS BLOCK" );
            return Application::EXIT_CONFIG;
        }
        
        if ( dump.size ()) {
            this->mMinerActivity->getLedger ().dump ( dump );
            return Application::EXIT_OK;
        }
        
        LGN_LOG ( VOL_FILTER_APP, INFO, "LOADING KEY FILE: %s", keyfile.c_str ());
        if ( !FileSys::exists ( keyfile )) {
            LGN_LOG ( VOL_FILTER_APP, ERROR, "...BUT THE FILE DOES NOT EXIST!" );
            return Application::EXIT_CONFIG;
        }
        
        this->mMinerActivity->loadKey ( keyfile );
        this->mMinerActivity->affirmKey ();
        this->mMinerActivity->affirmVisage ();
        this->mMinerActivity->setVerbose ();
        this->mMinerActivity->setReportMode ( Miner::REPORT_ALL_BRANCHES );
        this->mMinerActivity->setFixedUpdateDelayInMillis (( u32 )sleepFixed );
        this->mMinerActivity->setVariableUpdateDelayInMillis (( u32 )sleepVariable );
        this->mMinerActivity->setBlockTreeCacheSize (( size_t )blockTreeCacheSize );
        this->mMinerActivity->setConsensusLookaheadHeight (( size_t )consensusLookaheadHeight );
        this->mMinerActivity->setMaxBlockSearches (( size_t )blockSearchMax );
        
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

    //----------------------------------------------------------------//
    static Lognosis::Verbosity stringToLogLevel ( string logLevel ) {
        switch ( FNV1a::hash_64 ( logLevel.c_str ())) {
            case FNV1a::const_hash_64 ( "max" ):        return Lognosis::MAX;
            case FNV1a::const_hash_64 ( "info" ):       return Lognosis::INFO;
            case FNV1a::const_hash_64 ( "warn" ):       return Lognosis::WARNING;
            case FNV1a::const_hash_64 ( "error" ):      return Lognosis::ERROR;
            case FNV1a::const_hash_64 ( "fatal" ):      return Lognosis::FATAL;
            case FNV1a::const_hash_64 ( "off" ):        return Lognosis::OFF;
        }
        return Lognosis::OFF;
    }
    
    //----------------------------------------------------------------//
    static SQLiteConfig::JournalMode stringToJournalMode ( string journalMode ) {
        switch ( FNV1a::hash_64 ( journalMode.c_str ())) {
            case FNV1a::const_hash_64 ( "rollback" ):   return SQLiteConfig::JOURNAL_MODE_ROLLBACK;
            case FNV1a::const_hash_64 ( "wal" ):        return SQLiteConfig::JOURNAL_MODE_WAL;
        }
        return SQLiteConfig::JOURNAL_MODE_UNKNOWN;
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

    Lognosis::init ( argc, argv );

    sApp = make_shared < ServerApp >();
    int result = sApp->run ( argc, argv );
    sApp = NULL;
    return result;
}
