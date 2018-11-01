// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_TRANSACTIONS_H
#define VOLITION_TRANSACTION_TRANSACTIONS_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/AbstractSingleSignerTransaction.h>
#include <volition/Policy.h>

// ACCOUNT_POLICY (account, policy)
// AFFIRM_KEY (account name, key name, policy name, (opt)key)
// DELETE_KEY (account name, key name)
// GENESIS_MINER ( account name, key name, url, amount)
// KEY_POLICY (account, policy, policy name)
// OPEN_ACCOUNT (account name, master key, key name)
// PUBLISH_SCHEMA (json, lua)
// REGISTER_MINER (account name, url)
// SELL_ASSETS (assets, buyer, seller, amount)
// SEND_ASSETS (assets, from, to)
// SEND_VOL (from, to)
// TRANSFORM_ASSETS (...)

namespace Volition {
namespace Transaction {

//================================================================//
// AccountPolicy
//================================================================//
class AccountPolicy :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "ACCOUNT_POLICY" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mPolicyName;
    SerializableUniquePtr < Policy >        mPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        return true;
    }
};

//================================================================//
// AffirmKey
//================================================================//
class AffirmKey :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "AFFIRM_KEY" )
    TRANSACTION_WEIGHT ( 1 )

    string      mKeyName;
    CryptoKey   mKey;
    string      mPolicyName;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
    
        return ledger.affirmKey ( this->mMakerSignature->getAccountName (), this->mKeyName, this->mKey, this->mPolicyName );
    }
};

//================================================================//
// GenesisMiner
//================================================================//
class GenesisMiner :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "GENESIS_MINER" )
    TRANSACTION_WEIGHT ( 1 )

    string      mAccountName;
    CryptoKey   mKey;
    string      mKeyName;
    u64         mAmount;
    string      mURL;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "url",           this->mURL );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
        serializer.serialize ( "url",           this->mURL );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
    
        assert ( this->mKey );
        return ledger.genesisMiner ( this->mAccountName, this->mAmount, this->mKeyName, this->mKey, this->mURL );
    }
};

//================================================================//
// KeyPolicy
//================================================================//
class KeyPolicy :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "KEY_POLICY" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mPolicyName;
    SerializableUniquePtr < Policy >        mPolicy;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "policy",        this->mPolicy );
        serializer.serialize ( "policyName",    this->mPolicyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        return true;
    }
};

//================================================================//
// OpenAccount
//================================================================//
class OpenAccount :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "OPEN_ACCOUNT" )
    TRANSACTION_WEIGHT ( 1 )

    string      mAccountName;   // user provided name of account
    CryptoKey   mKey;           // master key
    string      mKeyName;       // name of master key
    u64         mAmount;        // amount to fund

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
        serializer.serialize ( "key",           this->mKey );
        serializer.serialize ( "keyName",       this->mKeyName );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        assert ( this->mKey );
        return ledger.openAccount ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount, this->mKeyName, this->mKey );
    }
};

//================================================================//
// PublishSchema
//================================================================//
class PublishSchema :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "PUBLISH_SCHEMA" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mSchemaName;
    string                                  mJSON;
    string                                  mLua;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "name",          this->mSchemaName );
        serializer.serialize ( "json",          this->mJSON );
        serializer.serialize ( "lua",           this->mLua );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "name",          this->mSchemaName );
        serializer.serialize ( "json",          this->mJSON );
        serializer.serialize ( "lua",           this->mLua );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
    
        return ledger.publishSchema ( this->mSchemaName, this->mJSON, this->mLua );
    }
};

//================================================================//
// RegisterMiner
//================================================================//
class RegisterMiner :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "REGISTER_MINER" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mURL;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "url",           this->mURL );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
    
        return ledger.registerMiner ( this->mMakerSignature->getAccountName (), this->mMakerSignature->getKeyName (), this->mURL );
    }
};

//================================================================//
// SellAsset
//================================================================//
class SellAssets :
    public AbstractTransaction {
public:

    TRANSACTION_TYPE ( "SELL_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )

    SerializableUniquePtr < TransactionMakerSignature >     mBuyerSignature;
    SerializableUniquePtr < TransactionMakerSignature >     mSellerSignature;

    SerializableVector < AssetIdentifier >                  mAssetIdentifiers;
    SerializableVector < BulkAssetIdentifier >              mBulkAssetItdentifiers;

    u64                                                     mPrice;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "buyerSignature",            this->mBuyerSignature );
        serializer.serialize ( "sellerSignature",           this->mSellerSignature );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
        serializer.serialize ( "bulkAssetIdentifiers",      this->mBulkAssetItdentifiers  );
        serializer.serialize ( "price",                     this->mPrice  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "buyerSignature",            this->mBuyerSignature );
        serializer.serialize ( "sellerSignature",           this->mSellerSignature );
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
        serializer.serialize ( "bulkAssetIdentifiers",      this->mBulkAssetItdentifiers  );
        serializer.serialize ( "price",                     this->mPrice  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        //return ledger.sendVOL ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount );
        return false;
    }
};

//================================================================//
// SendAssets
//================================================================//
class SendAssets :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "SEND_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )

    SerializableVector < AssetIdentifier >          mAssetIdentifiers;
    SerializableVector < BulkAssetIdentifier >      mBulkAssetItdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
        serializer.serialize ( "bulkAssetIdentifiers",      this->mBulkAssetItdentifiers  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "assetIdentifiers",          this->mAssetIdentifiers );
        serializer.serialize ( "bulkAssetIdentifiers",      this->mBulkAssetItdentifiers  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        //return ledger.sendVOL ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount );
        return false;
    }
};

//================================================================//
// SendVOL
//================================================================//
class SendVOL :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "SEND_VOL" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mAccountName; // receiving account
    u64                                     mAmount;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "accountName",   this->mAccountName );
        serializer.serialize ( "amount",        this->mAmount  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        return ledger.sendVOL ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount );
    }
};

//================================================================//
// TransformAssets
//================================================================//
class TransformAssets :
    public AbstractSingleSignerTransaction {
public:

    TRANSACTION_TYPE ( "TRANSFORM_ASSETS" )
    TRANSACTION_WEIGHT ( 1 )

    string                                  mSchemaName;
    string                                  mRuleName;
    SerializableVector < AssetIdentifier >  mAssetIdentifiers;

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeFrom ( serializer );
        
        serializer.serialize ( "schemaName",            this->mSchemaName );
        serializer.serialize ( "ruleName",              this->mRuleName );
        serializer.serialize ( "assetIdentifiers",      this->mAssetIdentifiers  );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        AbstractSingleSignerTransaction::AbstractSerializable_serializeTo ( serializer );
        
        serializer.serialize ( "schemaName",            this->mSchemaName );
        serializer.serialize ( "ruleName",              this->mRuleName );
        serializer.serialize ( "assetIdentifiers",      this->mAssetIdentifiers  );
    }

    //----------------------------------------------------------------//
    bool AbstractTransaction_apply ( Ledger& ledger ) const override {
        
        //return ledger.sendVOL ( this->mMakerSignature->getAccountName (), this->mAccountName, this->mAmount );
        return false;
    }
};

} // namespace Transaction
} // namespace Volition
#endif
