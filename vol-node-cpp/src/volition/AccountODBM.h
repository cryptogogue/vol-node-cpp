// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ACCOUNTODBM_H
#define VOLITION_ACCOUNTODBM_H

#include <volition/common.h>
#include <volition/Account.h>
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
    static LedgerKey keyFor_assetCount ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.assetCount", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_body ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryField ( AssetID::Index index, size_t position ) {
        return Format::write ( "account.%d.assets.%d", index, position );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryLogEntry ( AssetID::Index index, u64 inventoryNonce ) {
        return Format::write ( "account.%d.inventoryLog.%d", index, inventoryNonce );
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_inventoryNonce ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.inventoryNonce", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_minerHeight ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.minerHeight", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_minerInfo ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.miner", index ); });
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_name ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.name", index ); });
    }
    
    //----------------------------------------------------------------//
    static LedgerKey keyFor_transactionLookup ( AssetID::Index index, string uuid ) {
        return Format::write ( "account.%d.transactionLookup.%s", index, uuid.c_str ());
    }

    //----------------------------------------------------------------//
    static LedgerKey keyFor_transactionNonce ( AssetID::Index index ) {
        return LedgerKey ([ = ]() { return Format::write ( "account.%d.transactionNonce", index ); });
    }

    //----------------------------------------------------------------//
    void initialize ( ConstOpt < Ledger > ledger, AccountID index ) {
    
        this->mLedger       = ledger;
        this->mAccountID    = index;
        
        mAssetCount         = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_assetCount ( this->mAccountID ),             0 );
        mInventoryNonce     = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_inventoryNonce ( this->mAccountID ),         0 );
        mTransactionNonce   = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_transactionNonce ( this->mAccountID ),       0 );
        mName               = LedgerFieldODBM < string >( this->mLedger,                keyFor_name ( this->mAccountID ),                   "" );
        mBody               = LedgerObjectFieldODBM < Account >( this->mLedger,         keyFor_body ( this->mAccountID ));
        mMinerHeight        = LedgerFieldODBM < u64 >( this->mLedger,                   keyFor_minerHeight ( this->mAccountID ),            0 );
        mMinerInfo          = LedgerObjectFieldODBM < MinerInfo >( this->mLedger,       keyFor_minerInfo ( this->mAccountID ));
    }

public:

    ConstOpt < Ledger >     mLedger;
    AccountID               mAccountID;

    LedgerFieldODBM < u64 >                 mAssetCount;
    LedgerFieldODBM < u64 >                 mInventoryNonce;
    LedgerFieldODBM < u64 >                 mTransactionNonce;
    LedgerFieldODBM < string >              mName;

    LedgerObjectFieldODBM < Account >       mBody;
    LedgerFieldODBM < u64 >                 mMinerHeight;
    LedgerObjectFieldODBM < MinerInfo >     mMinerInfo;

    //----------------------------------------------------------------//
    operator bool () {
        return this->mName.exists ();
    }

    //----------------------------------------------------------------//
    AccountODBM ( ConstOpt < Ledger > ledger, AccountID index ) {
        this->initialize ( ledger, index );
    }
    
    //----------------------------------------------------------------//
    AccountODBM ( ConstOpt < Ledger > ledger, string accountName ) {
        this->initialize ( ledger, ledger.getConst ().getAccountID ( accountName ));
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
    LedgerFieldODBM < u64 > getTransactionLookupField ( string uuid ) {
    
        return LedgerFieldODBM < u64 >( this->mLedger, keyFor_transactionLookup ( this->mAccountID, uuid ), 0 );
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
};

} // namespace Volition
#endif
