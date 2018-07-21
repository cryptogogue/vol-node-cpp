// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

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

    Account* account = this->getAccount ( accountName );
    if ( account ) {

        if ( key ) {
            account->mKeys [ keyName ] = KeyAndPolicy ( key );
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------//
bool State::checkMakerSignature ( const TransactionMakerSignature* makerSignature ) const {

    if ( makerSignature ) {
        const Account* account = this->getAccount ( makerSignature->getAccountName ());
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
    
        Account* account = this->getAccount ( makerSignature->getAccountName ());
        if ( account && ( account->mNonce <= nonce )) {
            account->mNonce = nonce + 1;
        }
    }
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
bool State::genesisMiner ( string accountName, u64 amount, string keyName, const CryptoKey& key, string url ) {

    Account& account = this->mAccounts [ accountName ];
    account.mBalance = amount;
    account.mKeys [ keyName ] = KeyAndPolicy ( key );
    
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
        map < string, KeyAndPolicy >::iterator keyAndPolicyIt = accountKey.mAccount->mKeys.find ( keyName );
        if ( keyAndPolicyIt != accountKey.mAccount->mKeys.end ()) {
            accountKey.mKeyAndPolicy = &keyAndPolicyIt->second;
        }
    }
    return accountKey;
}

//----------------------------------------------------------------//
u64 State::getHeight () const {

    return this->mHeight;
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
bool State::openAccount ( string accountName, string recipientName, u64 amount, string keyName, const CryptoKey& key ) {

    Account* account = this->getAccount ( accountName );
    if ( account && ( account->mBalance >= amount )) {
        
        if ( this->getAccount ( recipientName )) return false;
        
        Account& recipient = this->mAccounts [ recipientName ];
        
        account->mBalance -= amount;
        recipient.mBalance = amount;
        
        recipient.mKeys [ keyName ] = KeyAndPolicy ( key );
        
        return true;
    }
    return false;
}

//----------------------------------------------------------------//
bool State::registerMiner ( string accountName, string keyName, string url ) {

    AccountKey accountKey = this->getAccountKey ( accountName, keyName );
    if ( accountKey ) {
        
        this->mMinerInfo [ accountName ] = MinerInfo ( accountName, url, accountKey.mKeyAndPolicy->mKey );
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
void State::setHeight ( u64 height ) {

    this->mHeight = height;
}

//----------------------------------------------------------------//
State::State () :
    mHeight ( 0 ) {
}

//----------------------------------------------------------------//
State::~State () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//

} // namespace Volition
