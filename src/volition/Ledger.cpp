// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Runtime.h>
#include <volition/Format.h>
#include <volition/Ledger.h>
#include <volition/TransactionMakerSignature.h>

namespace Volition {

//================================================================//
// Ledger
//================================================================//

//----------------------------------------------------------------//
bool Ledger::accountPolicy ( string accountName, const Policy* policy ) {

    return true;
}

//----------------------------------------------------------------//
bool Ledger::affirmKey ( string accountName, string keyName, const CryptoKey& key, string policyName ) {

    VersionedValue < Account > account = this->getAccount ( accountName );
    if ( account ) {

        if ( key ) {
            Account updatedAccount = *account;
            updatedAccount.mKeys [ keyName ] = KeyAndPolicy ( key );
            this->setAccount ( accountName, updatedAccount );
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::checkMakerSignature ( const TransactionMakerSignature* makerSignature ) const {

    // TODO: actually check maker signature

    if ( makerSignature ) {
        VersionedValue < Account > account = this->getAccount ( makerSignature->getAccountName ());
        if ( account ) {
            return ( account->mNonce == makerSignature->getNonce ());
        }
    }
    return true;
}

//----------------------------------------------------------------//
bool Ledger::deleteKey ( string accountName, string keyName ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {
        Account updatedAccount = *accountKey.mAccount;
        updatedAccount.mKeys.erase ( keyName );
        this->setAccount ( accountName, updatedAccount );
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool Ledger::genesisMiner ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url ) {

    Account account;

    account.mBalance = amount;
    account.mKeys [ keyName ] = KeyAndPolicy ( key );
    this->setAccount ( accountName, account );

    return this->registerMiner ( accountName, keyName, url );
}

//----------------------------------------------------------------//
AccountKey Ledger::getAccountKey ( string accountName, string keyName ) const {

    AccountKey accountKey;
    accountKey.mKeyAndPolicy = NULL;

    accountKey.mAccount = this->getAccount ( accountName );
    if ( accountKey.mAccount ) {
        map < string, KeyAndPolicy >::const_iterator keyAndPolicyIt = accountKey.mAccount->mKeys.find ( keyName );
        if ( keyAndPolicyIt != accountKey.mAccount->mKeys.cend ()) {
            accountKey.mKeyAndPolicy = &keyAndPolicyIt->second;
        }
    }
    return accountKey;
}

//----------------------------------------------------------------//
VersionedValue < Account > Ledger::getAccount ( string accountName ) const {

    return VersionedValue < Account >( *this, ( prefixKey ( ACCOUNT, accountName )));
}

//----------------------------------------------------------------//
VersionedValue < MinerInfo > Ledger::getMinerInfo ( string accountName ) const {

    return VersionedValue < MinerInfo >( *this, ( prefixKey ( MINER_INFO, accountName )));
}

//----------------------------------------------------------------//
map < string, MinerInfo > Ledger::getMiners () const {

    map < string, MinerInfo > minerInfoMap;

    const set < string >& miners = this->getValue < set < string >>( MINERS );
    set < string >::const_iterator minerIt = miners.cbegin ();
    for ( ; minerIt != miners.cend (); ++minerIt ) {
    
        const string& minerID = *minerIt;
        
        VersionedValue < MinerInfo > minerInfo = this->getMinerInfo ( minerID );
        assert ( minerInfo );
        minerInfoMap [ minerID ] = *minerInfo;
    }
    return minerInfoMap;
}

//----------------------------------------------------------------//
Ledger::MinerURLMap Ledger::getMinerURLs () const {

    return MinerURLMap ( *this, MINER_URLS );
}

//----------------------------------------------------------------//
string Ledger::getSchemaKey ( int schemaCount ) {

    return Format::write ( "%s%d", SCHEMA_PREFIX, schemaCount );
}

//----------------------------------------------------------------//
list < Schema > Ledger::getSchemas () const {

    list < Schema > schemaList;
    const int schemaCount = this->getValue < int >( SCHEMA_COUNT );
    for ( int i = 0; i < schemaCount; ++i ) {
        const Schema& schema = this->getValue < Schema >( Ledger::getSchemaKey ( i ));
        schemaList.push_back ( schema );
    }
    return schemaList;
}

//----------------------------------------------------------------//
void Ledger::incrementNonce ( const TransactionMakerSignature* makerSignature ) {

    if ( makerSignature ) {

        u64 nonce = makerSignature->getNonce ();
        string accountName = makerSignature->getAccountName ();

        VersionedValue < Account > account = this->getAccount ( accountName );
        if ( account && ( account->mNonce <= nonce )) {
            Account updatedAccount = *account;
            updatedAccount.mNonce = nonce + 1;
            this->setAccount ( accountName, updatedAccount );
        }
    }
}

//----------------------------------------------------------------//
bool Ledger::keyPolicy ( string accountName, string policyName, const Policy* policy ) {

    return true;
}

//----------------------------------------------------------------//
bool Ledger::openAccount ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key ) {

    VersionedValue < Account > account = this->getAccount ( accountName );
    if ( account && ( account->mBalance >= amount )) {

        if ( this->getAccount ( recipientName )) return false;

        Account accountUpdated = *account;
        accountUpdated.mBalance -= amount;
        this->setAccount ( accountName, accountUpdated );
        
        Account recipient;
        recipient.mBalance = amount;
        recipient.mKeys [ keyName ] = KeyAndPolicy ( key );
        this->setAccount ( recipientName, recipient );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
string Ledger::prefixKey ( string prefix, string key ) {

    return prefix + "." + key;
}

//----------------------------------------------------------------//
bool Ledger::publishSchema ( string schemaName, string json, string lua ) {

    if ( this->hasValue < string >( schemaName )) return false;

    int schemaCount = this->getValue < int >( SCHEMA_COUNT );
        
    Schema schema;
    schema.mJSON = json;
    schema.mLua = lua;
    
    string schemaKey = Ledger::getSchemaKey ( schemaCount );
    
    this->setValue < Schema >( schemaKey, schema );
    this->setValue < string >( schemaName, schemaKey );
    this->setValue < int >( SCHEMA_COUNT, schemaCount + 1 );
    
    Runtime runtime;
    runtime.loadScript ( lua );
    runtime.publish ( *this );
    
    return true;
}

//----------------------------------------------------------------//
bool Ledger::registerMiner ( string accountName, string keyName, string url ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {

        this->setValue < MinerInfo >( prefixKey ( MINER_INFO, accountName ), MinerInfo ( accountName, url, accountKey.mKeyAndPolicy->mKey ));
        
        // TODO: find an efficient way to do all this
        map < string, string > minerURLs = this->getValue < map < string, string >>( MINER_URLS );
        minerURLs [ accountName ] = url;
        this->setValue < map < string, string >>( MINER_URLS, minerURLs );

        set < string > miners = this->getValue < set < string >>( MINERS );
        miners.insert ( accountName );
        this->setValue < set < string >>( MINERS, miners );

        return true;
    }
    return false;
}

//----------------------------------------------------------------//
void Ledger::reset () {

    this->clear ();
    this->setValue < set < string >>( MINERS, set < string > ());
    this->setValue < map < string, string >>( MINER_URLS, map < string, string > ());
    this->setValue < int >( SCHEMA_COUNT, 0 );
}

//----------------------------------------------------------------//
bool Ledger::sendVOL ( string accountName, string recipientName, u64 amount ) {

    VersionedValue < Account > account      = this->getAccount ( accountName );
    VersionedValue < Account > recipient    = this->getAccount ( recipientName );

    if ( account && recipient && ( account->mBalance >= amount )) {
    
        Account accountUpdated = *account;
        Account recipientUpdated = *recipient;
    
        accountUpdated.mBalance -= amount;
        recipientUpdated.mBalance += amount;
        
        this->setAccount ( accountName, accountUpdated );
        this->setAccount ( recipientName, recipientUpdated );
        
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
void Ledger::setAccount ( string accountName, const Account& account ) {

    this->setValue < Account >( prefixKey ( ACCOUNT, accountName ), account );
}

//----------------------------------------------------------------//
void Ledger::setMinerInfo ( string accountName, const MinerInfo& minerInfo ) {

    this->setValue < MinerInfo >( prefixKey ( ACCOUNT, accountName ), minerInfo );
}

//----------------------------------------------------------------//
Ledger::Ledger () {

    this->reset ();
}

//----------------------------------------------------------------//
Ledger::Ledger ( Ledger& other ) :
    VersionedStore ( other ) {
}

//----------------------------------------------------------------//
Ledger::~Ledger () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
