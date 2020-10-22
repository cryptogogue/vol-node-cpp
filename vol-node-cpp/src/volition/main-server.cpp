// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <padamose/padamose.h>
#include <volition/Block.h>
#include <volition/FileSys.h>
#include <volition/RouteTable.h>
#include <volition/SimpleChainRecorder.h>
#include <volition/SQLiteChainRecorder.h>
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
            Poco::Util::Option ( "genesis", "g", "path to the genesis block" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "genesis" )
        );
        
        options.addOption (
            Poco::Util::Option ( "interval", "i", "set update interval (in seconds)" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "interval" )
        );
        
        options.addOption (
            Poco::Util::Option ( "keyfile", "k", "path to key file" )
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
                .required ( true )
                .argument ( "value", true )
                .binding ( "miner" )
        );
        
        options.addOption (
            Poco::Util::Option ( "nodelist", "n", "path to nodelist file" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "nodelist" )
        );
        
        options.addOption (
            Poco::Util::Option ( "persist-mode", "", "persist mode ('simple', 'sqlite')" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "persist-mode" )
        );
        
        options.addOption (
            Poco::Util::Option ( "port", "p", "set port to serve from" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "port" )
        );
        
        options.addOption (
            Poco::Util::Option ( "redis-conf", "rc", "path to the redis conf" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "redis-conf" )
        );
        
        options.addOption (
            Poco::Util::Option ( "redis-host", "rh", "redis hostname" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "redis-host" )
        );
        
        options.addOption (
            Poco::Util::Option ( "redis-folder", "rf", "path to the redis folder" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "redis-folder" )
        );
        
        options.addOption (
            Poco::Util::Option ( "redis-port", "rp", "redis port" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "redis-port" )
        );
        
        options.addOption (
            Poco::Util::Option ( "simple-recorder-folder", "srf", "path to simple recorder folder" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "simple-recorder-folder" )
        );
        
        options.addOption (
            Poco::Util::Option ( "solo", "s", "operate in solo mode" )
                .required ( false )
                .argument ( "value" )
                .binding ( "solo" )
        );
        
        options.addOption (
            Poco::Util::Option ( "sqlite-db-file", "", "sqlite db filename" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "sqlite-db-file" )
        );
    }

    //----------------------------------------------------------------//
    int main ( const vector < string >& ) override {
        
        printf ( "APPLICATION NAME: %s\n", this->name ());
        
        Poco::Util::AbstractConfiguration& configuration = this->config ();
        
        string configfile = configuration.getString ( "config", "" );
        
        if ( configfile.size () > 0 ) {
            this->loadConfiguration ( configfile, PRIO_APPLICATION - 1 );
        }
        else {
            this->loadConfiguration ( PRIO_APPLICATION - 1 );
        }
//      this->printProperties ();
        
        string controlKeyfile           = configuration.getString   ( "control-key", "" );
        string controlLevel             = configuration.getString   ( "control-level", "" );
        string genesis                  = configuration.getString   ( "genesis" );
        int interval                    = configuration.getInt      ( "interval", MinerActivity::DEFAULT_UPDATE_INTERVAL );
        string keyfile                  = configuration.getString   ( "keyfile", "" );
        string logpath                  = configuration.getString   ( "logpath", "" );
        string minerID                  = configuration.getString   ( "miner", "" );
        string nodelist                 = configuration.getString   ( "nodelist", "" );
        string persistMode              = configuration.getString   ( "persist-mode", "" );
        string persistFolder            = configuration.getString   ( "persist-folder", "persist-chain" );
        int port                        = configuration.getInt      ( "port", 9090 );
        string redisConf                = configuration.getString   ( "redis-conf", "./redis.conf" );
        string redisHost                = configuration.getString   ( "redis-conf", "127.0.0.1" );
        string redisFolder              = configuration.getString   ( "redis-folder", "./redis" );
        int redisPort                   = configuration.getInt      ( "redis-port", 0 );
        bool solo                       = configuration.getBool     ( "solo", false );
        string sqliteDBFile             = configuration.getString   ( "sqlite-db-file", "" );
        
        string sslCertFile              = configuration.getString   ( "openSSL.server.certificateFile", "" );
        
        if ( logpath.size () > 0 ) {
            freopen ( Format::write ( "%s/%s.log", logpath.c_str (), minerID.c_str ()).c_str (), "w+", stdout );
            freopen ( Format::write ( "%s/%s.err", logpath.c_str (), minerID.c_str ()).c_str (), "w+", stderr );
        }
        
        Padamose::RedisServerProc redisServerProc;
        shared_ptr < StringStorePersistenceProvider > persistenceProvider;
        
        if ( redisPort != 0 ) {
            redisServerProc.start ( redisFolder, redisConf, redisHost, redisPort );
            assert ( redisServerProc.getStatus () != RedisServerProc::NOT_RUNNING );
            
            shared_ptr < RedisStringStore > stringStore = redisServerProc.makeStringStore ();
            assert ( stringStore );
            
            persistenceProvider = make_shared < StringStorePersistenceProvider >( stringStore );
        }
        
        shared_ptr < MinerActivity > minerActivity = make_shared < MinerActivity >();
        
        minerActivity->setMinerID ( minerID );
        
        if ( controlKeyfile.size ()) {
        
            CryptoPublicKey controlKey;
            FromJSONSerializer::fromJSONFile ( controlKey, controlKeyfile );
            if ( !controlKey ) {
                LOG_F ( INFO, "CONTROL KEY NOT FOUND: %s", controlKeyfile.c_str ());
                return Application::EXIT_CONFIG;
            }
            LOG_F ( INFO, "CONTROL KEY: %s", controlKeyfile.c_str ());
            minerActivity->setControlKey ( controlKey );
        }
        
        if ( controlLevel == "config" ) {
            LOG_F ( INFO, "CONTROL LEVEL: CONFIG" );
            minerActivity->setControlLevel ( Miner::CONTROL_CONFIG );
        }
        
        if ( controlLevel == "admin" ) {
            LOG_F ( INFO, "CONTROL LEVEL: ADMIN" );
            minerActivity->setControlLevel ( Miner::CONTROL_ADMIN );
        }
    
        if ( solo ) {
            LOG_F ( INFO, "LAZY and SOLO" );
            minerActivity->setLazy ( true );
        }
        
        minerActivity->setUpdateInterval (( u32 )interval );
        
        LOG_F ( INFO, "LOADING GENESIS BLOCK: %s", genesis.c_str ());
        if ( !FileSys::exists ( genesis )) {
            LOG_F ( INFO, "...BUT THE FILE DOES NOT EXIST!" );
            return Application::EXIT_CONFIG;
        }
        minerActivity->loadGenesis ( genesis );
        
        if ( persistMode == "simple" ) {
            shared_ptr < AbstractChainRecorder > chainRecorder = make_shared < SimpleChainRecorder >( *minerActivity, persistFolder );
            minerActivity->setChainRecorder ( chainRecorder );
        }
        
        if ( persistMode == "sqlite" ) {
            shared_ptr < AbstractChainRecorder > chainRecorder = make_shared < SQLiteChainRecorder >( *minerActivity, persistFolder );
            minerActivity->setChainRecorder ( chainRecorder );
        }
        
        if ( keyfile.size () > 0 ) {
            LOG_F ( INFO, "LOADING KEY FILE: %s\n", keyfile.c_str ());
            if ( !FileSys::exists ( keyfile )) {
                LOG_F ( INFO, "...BUT THE FILE DOES NOT EXIST!" );
                return Application::EXIT_CONFIG;
            }
            minerActivity->loadKey ( keyfile );
        }
        minerActivity->affirmKey ();
        minerActivity->affirmVisage ();
        
        LOG_F ( INFO, "MINER ID: %s", minerActivity->getMinerID ().c_str ());

        this->serve ( minerActivity, port, sslCertFile.length () > 0 );
        
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
                //logger ().information ( msg );
                printf ( "%s\n", msg.c_str ());
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
    void serve ( shared_ptr < MinerActivity > minerActivity, int port, bool ssl ) {

        Poco::ThreadPool threadPool;

        Poco::Net::HTTPServer server (
            new MinerAPIFactory ( minerActivity ),
            threadPool,
            ssl ? Poco::Net::SecureServerSocket (( Poco::UInt16 )port ) : Poco::Net::ServerSocket (( Poco::UInt16 )port ),
            new Poco::Net::HTTPServerParams ()
        );
        
        server.start ();
        minerActivity->start ();

        LOG_F ( INFO, "\nSERVING YOU BLOCKCHAIN REALNESS ON PORT: %d\n", port );

        // nasty little hack. POCO considers the set breakpoint signal to be a termination event.
        // need to find out how to stop POCO from doing this. in the meantime, this hack.
        #ifdef _DEBUG
            minerActivity->waitForShutdown ();
        #else
            this->waitForTerminationRequest ();  // wait for CTRL-C or kill
        #endif

        server.stop ();
        threadPool.stopAll ();
        
        {
            ScopedMinerLock scopedLock ( minerActivity );
            minerActivity->shutdown ( false );
        }
    }

public:

    //----------------------------------------------------------------//
    ServerApp () {
    
        Poco::Net::initializeSSL ();
    }
    
    //----------------------------------------------------------------//
    ~ServerApp () {
    
        Poco::Net::uninitializeSSL ();
    }
};

//================================================================//
// main
//================================================================//

//----------------------------------------------------------------//
int main ( int argc, char** argv ) {

    // force line buffering even when running as a spawned process
    setvbuf ( stdout, NULL, _IOLBF, 0 );
    setvbuf ( stderr, NULL, _IOLBF, 0 );

    Lognosis::setFilter ( PDM_FILTER_ROOT, Lognosis::OFF );
    Lognosis::init ( argc, argv );
    LOG_F ( INFO, "\nHello from VOLITION main.cpp!" );
    LOG_F ( INFO, "commit: %s", VOLITION_GIT_COMMIT_STR );
    LOG_F ( INFO, "build: %s %s", VOLITION_BUILD_DATE_STR, VOLITION_GIT_TAG_STR );

    ServerApp app;
    return app.run ( argc, argv );
}
