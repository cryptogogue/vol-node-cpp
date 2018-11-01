// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Format.h>
#include <volition/State.h>
#include <volition/TransactionMakerSignature.h>

namespace Volition {

//================================================================//
// State
//================================================================//

//----------------------------------------------------------------//
bool State::accountPolicy ( string accountName, const Policy* policy ) {

    return true;
}

//----------------------------------------------------------------//
bool State::affirmKey ( string accountName, string keyName, const CryptoKey& key, string policyName ) {

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
bool State::checkMakerSignature ( const TransactionMakerSignature* makerSignature ) const {

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
void State::consumeMakerSignature ( const TransactionMakerSignature* makerSignature ) {

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
bool State::deleteKey ( string accountName, string keyName ) {

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
bool State::genesisMiner ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url ) {

    Account account;

    account.mBalance = amount;
    account.mKeys [ keyName ] = KeyAndPolicy ( key );
    this->setAccount ( accountName, account );

    return this->registerMiner ( accountName, keyName, url );
}

//----------------------------------------------------------------//
AccountKey State::getAccountKey ( string accountName, string keyName ) const {

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
VersionedValue < Account > State::getAccount ( string accountName ) const {

    return VersionedValue < Account >( *this, ( prefixKey ( ACCOUNT, accountName )));
}

//----------------------------------------------------------------//
VersionedValue < MinerInfo > State::getMinerInfo ( string accountName ) const {

    return VersionedValue < MinerInfo >( *this, ( prefixKey ( MINER_INFO, accountName )));
}

//----------------------------------------------------------------//
map < string, MinerInfo > State::getMiners () const {

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
State::MinerURLMap State::getMinerURLs () const {

    return MinerURLMap ( *this, MINER_URLS );
}

//----------------------------------------------------------------//
string State::getSchemaKey ( int schemaCount ) {

    return Format::write ( "%s%d", SCHEMA_PREFIX, schemaCount );
}

//----------------------------------------------------------------//
list < Schema > State::getSchemas () const {

    list < Schema > schemaList;
    const int schemaCount = this->getValue < int >( SCHEMA_COUNT );
    for ( int i = 0; i < schemaCount; ++i ) {
        const Schema& schema = this->getValue < Schema >( State::getSchemaKey ( i ));
        schemaList.push_back ( schema );
    }
    return schemaList;
}

//----------------------------------------------------------------//
bool State::keyPolicy ( string accountName, string policyName, const Policy* policy ) {

    return true;
}

//----------------------------------------------------------------//
bool State::openAccount ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key ) {

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
string State::prefixKey ( string prefix, string key ) {

    return prefix + "." + key;
}

//----------------------------------------------------------------//
bool State::publishSchema ( string json, string lua ) {

    int schemaCount = this->getValue < int >( SCHEMA_COUNT );
        
    Schema schema;
    schema.mJSON = json;
    schema.mLua = lua;
    
    this->setValue < Schema >( State::getSchemaKey ( schemaCount ), schema );
    this->setValue < int >( SCHEMA_COUNT, schemaCount + 1 );
    
    return true;
}

//----------------------------------------------------------------//
bool State::registerMiner ( string accountName, string keyName, string url ) {

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
void State::reset () {

    this->clear ();
    this->setValue < set < string >>( MINERS, set < string > ());
    this->setValue < map < string, string >>( MINER_URLS, map < string, string > ());
    this->setValue < int >( SCHEMA_COUNT, 0 );
}

//----------------------------------------------------------------//
bool State::sendVOL ( string accountName, string recipientName, u64 amount ) {

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
void State::setAccount ( string accountName, const Account& account ) {

    this->setValue < Account >( prefixKey ( ACCOUNT, accountName ), account );
}

//----------------------------------------------------------------//
void State::setMinerInfo ( string accountName, const MinerInfo& minerInfo ) {

    this->setValue < MinerInfo >( prefixKey ( ACCOUNT, accountName ), minerInfo );
}

//----------------------------------------------------------------//
State::State () {

    this->reset ();
}

//----------------------------------------------------------------//
State::State ( State& other ) :
    VersionedStore ( other ) {
}

//----------------------------------------------------------------//
State::~State () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
