// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCOUNTODBM_H
#define VOLITION_ACCOUNTODBM_H

#include <volition/common.h>
#include <volition/Account.h>
#include <volition/AccountLogEntry.h>
#include <volition/Asset.h>
#include <volition/Ledger.h>
#include <volition/LedgerFieldODBM.h>
#include <volition/LedgerKey.h>
#include <volition/LedgerObjectFieldODBM.h>
#include <volition/Policy.h>
#include <volition/Schema.h>

namespace Volition {

//================================================================//
// AccountODBM
//================================================================//
class AccountODBM {
private:

    //----------------------------------------------------------------//
    static LedgerKey keyFor_accountLogEntry ( AccountID::Index index, u64 entry ) {
        return Format::write ( "account.%d.log.%d", index, entry );
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_accountLogSize ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.logSize", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_assetCount ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.assetCount", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_balance ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.balance", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_body ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_fingerprint ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.fingerprint", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryField ( AccountID::Index index, size_t position ) {
        return Format::write ( "account.%d.assets.%d", index, position );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_identityProviderName ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.identityProviderName", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryLogEntry ( AccountID::Index index, u64 inventoryNonce ) {
        return Format::write ( "account.%d.inventoryLog.%d", index, inventoryNonce );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryNonce ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.inventoryNonce", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_minerHeight ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.minerHeight", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_minerInfo ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.miner", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_minerBlockCount ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.minerBLockCount", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_minerRewardCount ( AccountID::Index index, string rewardName ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.minerRewardAcount.%s", index, rewardName.c_str ()); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_name ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.name", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_transactionLookup ( AccountID::Index index, string uuid ) {
        return Format::write ( "account.%d.transactionLookupByUUID.%s", index, uuid.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_transactionNonce ( AccountID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.transactionNonce", index ); });
    }

    //----------------------------------------------------------------//
    void initialize ( ConstOpt < AbstractLedger > ledger, AccountID index ) {
    
        this->mLedger       = ledger;
        this->mAccountID    = index;
        
        this->mAccountLogSize           = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_accountLogSize ( this->mAccountID ),         0 );
        this->mAssetCount               = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_assetCount ( this->mAccountID ),             0 );
        this->mBalance                  = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_balance ( this->mAccountID ),                0 );
        this->mFingerprint              = LedgerFieldODBM < string >( this->mLedger,                keyFor_fingerprint ( this->mAccountID ),            "" );
        this->mIdentityProviderName     = LedgerFieldODBM < string >( this->mLedger,                keyFor_identityProviderName ( this->mAccountID ),     "" );
        this->mInventoryNonce           = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_inventoryNonce ( this->mAccountID ),         0 );
        this->mTransactionNonce         = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_transactionNonce ( this->mAccountID ),       0 );
        this->mName                     = LedgerFieldODBM < string >( this->mLedger,                keyFor_name ( this->mAccountID ),                   "" );
        this->mBody                     = LedgerObjectFieldODBM < Account >( this->mLedger,         keyFor_body ( this->mAccountID ));
        this->mMinerHeight              = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_minerHeight ( this->mAccountID ),            0 );
        this->mMinerInfo                = LedgerObjectFieldODBM < MinerInfo >( this->mLedger,       keyFor_minerInfo ( this->mAccountID ));
        this->mMinerBlockCount          = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_minerBlockCount ( this->mAccountID ),        0 );
    }

public:

    ConstOpt < AbstractLedger >             mLedger;
    AccountID                               mAccountID;

    LedgerFieldODBM < u64 >                 mAccountLogSize;
    LedgerFieldODBM < u64 >                 mAssetCount;
    LedgerFieldODBM < u64 >                 mBalance;
    LedgerFieldODBM < string >              mFingerprint;
    LedgerFieldODBM < string >              mIdentityProviderName;
    LedgerFieldODBM < u64 >                 mInventoryNonce;
    LedgerFieldODBM < u64 >                 mTransactionNonce;
    LedgerFieldODBM < string >              mName;

    LedgerObjectFieldODBM < Account >       mBody;
    LedgerFieldODBM < u64 >                 mMinerHeight;
    LedgerObjectFieldODBM < MinerInfo >     mMinerInfo;
    LedgerFieldODBM < u64 >                 mMinerBlockCount;

    //----------------------------------------------------------------//
    operator bool () {
    
        return this->mName.exists ();
    }

    //----------------------------------------------------------------//
    AccountODBM ( ConstOpt < AbstractLedger > ledger, AccountID accountID ) {
    
        if ( accountID != AccountID::NULL_INDEX ) {
            this->initialize ( ledger, accountID );
        }
    }
    
    //----------------------------------------------------------------//
    AccountODBM ( ConstOpt < AbstractLedger > ledger, string accountName ) {
        
        this->initialize ( ledger, ledger.getConst ().getAccountID ( accountName ));
    }
    
    //----------------------------------------------------------------//
    void addFunds ( u64 amount ) {
    
        if ( amount > 0 ) {
            this->mBalance.set ( this->mBalance.get () + amount );
        }
    }
    
    //----------------------------------------------------------------//
    LedgerObjectFieldODBM < AccountLogEntry > getAccountLogEntryField ( u64 entryIndex ) {
    
        return LedgerObjectFieldODBM < AccountLogEntry >( this->mLedger, keyFor_accountLogEntry ( this->mAccountID, entryIndex ));
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM < AssetID::Index > getInventoryField ( size_t position ) {
    
        return LedgerFieldODBM < AssetID::Index >( this->mLedger, keyFor_inventoryField ( this->mAccountID, position ), AssetID::NULL_INDEX );
    }
    
    //----------------------------------------------------------------//
    LedgerObjectFieldODBM < InventoryLogEntry > getInventoryLogEntryField ( u64 inventoryNonce ) {
    
        return LedgerObjectFieldODBM < InventoryLogEntry >( this->mLedger, keyFor_inventoryLogEntry ( this->mAccountID, inventoryNonce ));
    }
    
    //----------------------------------------------------------------//
    KeyAndPolicy getKeyAndPolicyOrNull ( string keyName ) {
    
        if ( keyName.size ()) {
            return this->mBody.get ()->getKeyAndPolicy ( keyName );
        }
        
        shared_ptr < const MinerInfo > minerInfo = this->mMinerInfo.get ();
        if ( minerInfo ) {
        
            const CryptoPublicKey& key = minerInfo->getPublicKey ();
            Policy policy;
            policy.setRestrictions ( KeyEntitlements::getMiningKeyEntitlements ());
            return KeyAndPolicy ( key, policy );
        }
        return KeyAndPolicy ();
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM < u64 > getMinerRewardCountField ( string rewardName ) {
    
        return LedgerFieldODBM < u64 >( this->mLedger, keyFor_minerRewardCount ( this->mAccountID, rewardName ), 0 );
    }
    
    //----------------------------------------------------------------//
    LedgerFieldODBM < u64 > getTransactionLookupField ( string uuid ) {
    
        return LedgerFieldODBM < u64 >( this->mLedger, keyFor_transactionLookup ( this->mAccountID, uuid ), 0 );
    }
    
    //----------------------------------------------------------------//
    bool hasFunds ( u64 amount ) {
    
        return amount <= this->mBalance.get ();
    }
    
    //----------------------------------------------------------------//
    bool hasIdentity () {
    
        return (( this->mFingerprint.get ().size ()) > 0 && ( this->mIdentityProviderName.get ().size () > 0 ));
    }
    
    //----------------------------------------------------------------//
    void incAccountTransactionNonce ( u64 nonce, string uuid ) {
        
        if ( !( *this )) return;
        
        this->getTransactionLookupField ( uuid ).set ( nonce );
        this->mTransactionNonce.set ( nonce + 1 );
    }
    
    //----------------------------------------------------------------//
    bool isMiner () {
    
        return this->mMinerInfo.exists ();
    }
    
    //----------------------------------------------------------------//
    void setIdentity ( string fingerprint, string identityProviderName ) {
    
        this->mFingerprint.set ( fingerprint );
        this->mIdentityProviderName.set ( identityProviderName );
    }
    
    //----------------------------------------------------------------//
    void subFunds ( u64 amount ) {
    
        if ( amount > 0 ) {
            u64 balance = this->mBalance.get ();
            this->mBalance.set ( amount <= balance ? balance - amount : 0 );
        }
    }
};

} // namespace Volition
#endif
