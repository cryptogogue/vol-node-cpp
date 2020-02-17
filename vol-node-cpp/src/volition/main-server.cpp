// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <padamose/padamose.h>
#include <volition/Block.h>
#include <volition/FileSys.h>
#include <volition/RouteTable.h>
#include <volition/SimpleChainRecorder.h>
#include <volition/Singleton.h>
#include <volition/TheContext.h>
#include <volition/TheTransactionBodyFactory.h>
#include <volition/TheWebMiner.h>
#include <volition/web-miner-api/HTTPRequestHandlerFactory.h>

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
            Poco::Util::Option ( "port", "p", "set port to serve from" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "port" )
        );
        
        options.addOption (
            Poco::Util::Option ( "nodelist", "n", "path to nodelist file" )
                .required ( false )
                .argument ( "value", true )
                .binding ( "nodelist" )
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
        
        string genesis                  = configuration.getString   ( "genesis" );
        int interval                    = configuration.getInt      ( "interval", Volition::WebMiner::DEFAULT_UPDATE_INTERVAL );
        string keyfile                  = configuration.getString   ( "keyfile" );
        int port                        = configuration.getInt      ( "port", 9090 );
        string nodelist                 = configuration.getString   ( "nodelist", "" );
        string redisConf                = configuration.getString   ( "redis-conf", "./redis.conf" );
        string redisHost                = configuration.getString   ( "redis-conf", "127.0.0.1" );
        string redisFolder              = configuration.getString   ( "redis-folder", "./redis" );
        int redisPort                   = configuration.getInt      ( "redis-port", 0 );
        string simpleRecorderFolder     = configuration.getString   ( "simple-recorder-folder", "" );
        bool solo                       = configuration.getBool     ( "solo", false );
        string sslCertFile              = configuration.getString   ( "openSSL.server.certificateFile", "" );
        
        string minerID                  = to_string ( port );
    
        Volition::TheContext::get ().setScoringMode ( Volition::TheContext::ScoringMode::INTEGER );
        
        Padamose::RedisServerProc redisServerProc;
        shared_ptr < StringStorePersistenceProvider > persistenceProvider;
        
        if ( redisPort != 0 ) {
            redisServerProc.start ( redisFolder, redisConf, redisHost, redisPort );
            assert ( redisServerProc.getStatus () != RedisServerProc::NOT_RUNNING );
            
            shared_ptr < RedisStringStore > stringStore = redisServerProc.makeStringStore ();
            assert ( stringStore );
            
            persistenceProvider = make_shared < StringStorePersistenceProvider >( stringStore );
        }
        
        {
            Volition::ScopedWebMinerLock scopedLock ( Volition::TheWebMiner::get ());
            Volition::WebMiner& webMiner = scopedLock.getWebMiner ();
        
            if ( solo ) {
                LOG_F ( INFO, "LAZY and SOLO" );
                webMiner.setLazy ( true );
                webMiner.setSolo ( true );
            }
            
            webMiner.setUpdateInterval (( u32 )interval );
            
            LOG_F ( INFO, "LOADING GENESIS BLOCK: %s", genesis.c_str ());
            if ( !Volition::FileSys::exists ( genesis )) {
                LOG_F ( INFO, "...BUT THE FILE DOES NOT EXIST!" );
                return Application::EXIT_CONFIG;
            }
            webMiner.loadGenesis ( genesis );
            
            if ( simpleRecorderFolder.size () > 0 ) {
                shared_ptr < Volition::AbstractChainRecorder > chainRecorder = make_shared < Volition::SimpleChainRecorder >( webMiner, simpleRecorderFolder );
                webMiner.setChainRecorder ( chainRecorder );
            }
            
            LOG_F ( INFO, "LOADING KEY FILE: %s\n", keyfile.c_str ());
            if ( !Volition::FileSys::exists ( keyfile )) {
                LOG_F ( INFO, "...BUT THE FILE DOES NOT EXIST!" );
                return Application::EXIT_CONFIG;
            }
            webMiner.loadKey ( keyfile );
            
            LOG_F ( INFO, "MINER ID: %s", minerID.c_str ());
            webMiner.setMinerID ( minerID );
            webMiner.start ();
        }

        this->serve ( port, sslCertFile.length () > 0 );

        {
            Volition::ScopedWebMinerLock scopedLock ( Volition::TheWebMiner::get ());
            scopedLock.getWebMiner ().shutdown ();
        }
        return Application::EXIT_OK;
    }
    
    //----------------------------------------------------------------//
    const char* name () {
    
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
    void serve ( int port, bool ssl ) {


        Poco::Net::HTTPServer server (
            new Volition::WebMinerAPI::HTTPRequestHandlerFactory (),
            ssl ? Poco::Net::SecureServerSocket (( Poco::UInt16 )port ) : Poco::Net::ServerSocket (( Poco::UInt16 )port ),
            new Poco::Net::HTTPServerParams ()
        );
        server.start ();

        LOG_F ( INFO, "\nSERVING YOU BLOCKCHAIN REALNESS ON PORT: %d\n", port );

        // nasty little hack. POCO considers the set breakpoint signal to be a termination event.
        // need to find out how to stop POCO from doing this. in the meantime, this hack.
        #ifdef _DEBUG
            Poco::Event dummy;
            dummy.wait ();
        #else
            this->waitForTerminationRequest ();  // wait for CTRL-C or kill
        #endif

        server.stop ();
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
    LOG_F ( INFO, "\nHello from VOLITION main.cpp!\n" );

    ServerApp app;
    return app.run ( argc, argv );
}
