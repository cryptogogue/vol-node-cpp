//
//  main.cpp
// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>

namespace Volition {

//================================================================//
// State
//================================================================//

//----------------------------------------------------------------//
bool State::accountPolicy ( string accountName, const Policy* policy ) {

    return true;
}

//----------------------------------------------------------------//
bool State::affirmKey ( string accountName, string keyName, const Poco::Crypto::ECKey* key, string policyName ) {

    Account* account = this->getAccount ( accountName );
    if ( account ) {

        if ( key ) {
            account->mKeys.insert ( pair < string, KeyAndPolicy >( keyName, KeyAndPolicy ( *key ))); // because KeyAndPolicy has no empty constructor
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool State::deleteKey ( string accountName, string keyName ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {
        accountKey.mAccount->mKeys.erase ( keyName );
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool State::genesisMiner ( string accountName, u64 amount, string keyName, const Poco::Crypto::ECKey& key, string url ) {

    Account& account = this->mAccounts [ accountName ];
    account.mBalance = amount;
    account.mKeys.insert ( pair < string, KeyAndPolicy >( keyName, KeyAndPolicy ( key ))); // because KeyAndPolicy has no empty constructor
    
    this->registerMiner ( accountName, keyName, url );
    return true;
}

//----------------------------------------------------------------//
Account* State::getAccount ( string accountName ) {

    if ( this->mAccounts.find ( accountName ) != this->mAccounts.end ()) {
        return &this->mAccounts [ accountName ];
    }
    return NULL;
}

//----------------------------------------------------------------//
const Account* State::getAccount ( string accountName ) const {

    map < string, Account >::const_iterator accountIt = this->mAccounts.find ( accountName );
    if ( accountIt != this->mAccounts.cend ()) {
        return &accountIt->second;
    }
    return NULL;
}

//----------------------------------------------------------------//
AccountKey State::getAccountKey ( string accountName, string keyName ) {

    AccountKey accountKey;
    
    accountKey.mAccount         = NULL;
    accountKey.mKeyAndPolicy    = NULL;

    accountKey.mAccount = this->getAccount ( accountName );
    if ( accountKey.mAccount ) {
        map < string, KeyAndPolicy >::iterator keyAndPolicyIt = accountKey.mAccount->mKeys.find ( accountName );
        if ( keyAndPolicyIt != accountKey.mAccount->mKeys.end ()) {
            accountKey.mKeyAndPolicy = &keyAndPolicyIt->second;
        }
    }
    return accountKey;
}

//----------------------------------------------------------------//
const map < string, MinerInfo >& State::getMinerInfo () const {

    return this->mMinerInfo;
}

//----------------------------------------------------------------//
const MinerInfo* State::getMinerInfo ( string minerID ) const {

    map < string, MinerInfo >::const_iterator minerInfoIt = this->mMinerInfo.find ( minerID );
    if ( minerInfoIt != this->mMinerInfo.end ()) {
        return &minerInfoIt->second;
    }
    return NULL;
}

//----------------------------------------------------------------//
const map < string, string >& State::getMinerURLs () const {

    return this->mMinerURLs;
}

//----------------------------------------------------------------//
bool State::keyPolicy ( string accountName, string policyName, const Policy* policy ) {

    return true;
}

//----------------------------------------------------------------//
bool State::openAccount ( string accountName, string recipientName, u64 amount, string keyName, const Poco::Crypto::ECKey& key ) {

    Account* account = this->getAccount ( accountName );
    if ( account && ( account->mBalance >= amount )) {
        
        if ( this->getAccount ( recipientName )) return false;
        
        Account& recipient = this->mAccounts [ recipientName ];
        
        account->mBalance -= amount;
        recipient.mBalance = amount;
        
        recipient.mKeys.insert ( pair < string, KeyAndPolicy >( keyName, KeyAndPolicy ( key ))); // because KeyAndPolicy has no empty constructor
        
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool State::registerMiner ( string accountName, string keyName, string url ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {
        
        this->mMinerInfo.insert ( pair < string, MinerInfo >( accountName, MinerInfo ( accountName, url, accountKey.mKeyAndPolicy->mKey ) )); // because MinerInfo has no empty constructor
        this->mMinerURLs [ accountName ] = url;
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool State::sendVOL ( string accountName, string recipientName, u64 amount ) {

    Account* account    = this->getAccount ( accountName );
    Account* recipient  = this->getAccount ( recipientName );

    if ( account && recipient && ( account->mBalance >= amount )) {
        account->mBalance -= amount;
        recipient->mBalance += amount;
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
State::State () {
}

//----------------------------------------------------------------//
State::~State () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition