// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransactionBody.h>
#include <volition/AccountODBM.h>
#include <volition/Ledger.h>
#include <volition/Miner.h>
#include <volition/Transaction.h>
#include <volition/TransactionContext.h>
#include <volition/TransactionMaker.h>
#include <volition/transactions/Genesis.h>

namespace Volition {

//================================================================//
// Transaction
//================================================================//

//----------------------------------------------------------------//
TransactionResult Transaction::apply ( Ledger& ledger, time_t time, Block::VerificationPolicy policy ) const {

    try {
        TransactionResult result = this->checkBody ( ledger, time );
        if ( result ) {
            result = this->applyInner ( ledger, time, policy );
        }
        result.setTransactionDetails ( *this );
        return result;
    }
    catch ( const std::exception &e ) {
        return Format::write ( "Exception occured while applying transaction: %s", e.what ());
    }
    catch ( ... ) {
        return "Exception occured while applying transaction.";
    }
}

//----------------------------------------------------------------//
TransactionResult Transaction::applyInner ( Ledger& ledger, time_t time, Block::VerificationPolicy policy ) const {
    
    if ( !this->mBody ) return "Missing body.";
    
    TransactionMaker* maker = this->mBody->mMaker.get ();
    if ( !maker ) {
        return this->mBody->genesis ( ledger );
    }
    
    AccountODBM accountODBM ( ledger, maker->getAccountName ());
        
    if ( !accountODBM ) return "Transaction maker account not found.";
    
    KeyAndPolicy keyAndPolicy = accountODBM.getKeyAndPolicyOrNull ( maker->getKeyName ());
    if ( !keyAndPolicy ) return "Transaction maker key not found.";
    
    TransactionResult result = this->checkNonceAndSignature ( ledger, accountODBM.mAccountID, keyAndPolicy.mKey, policy );
    if ( result ) {
        
        TransactionContext context ( ledger, accountODBM, keyAndPolicy, time );
        
        const FeeProfile& feeProfile = context.mFeeSchedule.getFeeProfile ( this->getFeeName ());
        
        if ( !feeProfile.checkProfitShare ( maker->getGratuity (), maker->getProfitShare ())) return "Incorrect profit share.";
        if ( !feeProfile.checkTransferTax ( this->getSendVOL (), maker->getTransferTax ())) return "Incorrect transfer tax.";
        
        u64 cost = this->getCost ();
        if ( context.mAccount.mBalance < cost ) return "Insufficient funds.";
        
        result = this->mBody->apply ( context );
        
        if ( result ) {
            if ( !ledger.isGenesis ()) {
                
                accountODBM.incAccountTransactionNonce ( this->getNonce (), this->getUUID ());
                
                if ( cost > 0 ) {
                
                    // deduct the total cost from maker
                    Account accountUpdated = context.mAccount;
                    accountUpdated.mBalance -= cost;
                    accountODBM.mBody.set ( accountUpdated );
                }
            }
        }
    }
    return result;
}

//----------------------------------------------------------------//
TransactionResult Transaction::checkBody ( Ledger& ledger, time_t time ) const {

    if ( this->mBody->mUUID.size () > MAX_UUID_LENGTH ) return Format::write ( "Transaction UUID exceeds %d-character limit.", MAX_UUID_LENGTH );
    
    if ( this->mBody->mMaxHeight > 0 ) {
        u64 height = ledger.getHeight ();
        if ( height > this->mBody->mMaxHeight ) return Format::write ( "Transaction expired at chain height of %d.", this->mBody->mMaxHeight );
    }
    
    if ( this->mBody->mRecordBy > 0 ) {
        if ( time > this->mBody->mRecordBy ) return Format::write ( "Transaction expired at %s.", (( string )this->mBody->mRecordBy ).c_str ());
    }
    return true;
}

//----------------------------------------------------------------//
bool Transaction::checkMaker ( string accountName, string uuid ) const {

    if ( !this->mBody ) return false;
    if ( this->getUUID ().size () == 0 ) return false;
    TransactionMaker* maker = this->mBody->mMaker.get ();
    return ( maker && ( maker->getAccountName () == accountName ) && ( this->getUUID () == uuid ));
}

//----------------------------------------------------------------//
TransactionResult Transaction::checkNonceAndSignature ( const Ledger& ledger, AccountID accountID, const CryptoPublicKey& key, Block::VerificationPolicy policy ) const {

    if ( ledger.isGenesis ()) return true;
    if ( this->getUUID ().size () == 0 ) return false;

    u64 nonce = AccountODBM ( ledger, accountID ).mTransactionNonce.get ();
    if ( nonce != this->getNonce ()) return false;

    if ( policy & Block::VERIFY_TRANSACTION_SIG ) {
    
        Signature* signature = this->mSignature.get ();
        return signature ? key.verify ( *signature, this->mBodyString ) : false;
    }
    return true;
}

//----------------------------------------------------------------//
void Transaction::setBody ( shared_ptr < AbstractTransactionBody > body ) {

    this->mBody = body;
    this->mBodyString = body ? ToJSONSerializer::toJSONString ( *body ) : "";
}

//----------------------------------------------------------------//
void Transaction::sign ( const CryptoKeyPair& keyPair ) {

    this->mSignature = make_shared < Signature >();
    *this->mSignature = keyPair.sign ( this->mBodyString );
}

//----------------------------------------------------------------//
Transaction::Transaction () {
}

//----------------------------------------------------------------//
Transaction::~Transaction () {
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void Transaction::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    serializer.serialize ( "signature", this->mSignature );
    
    if ( serializer.has ( "bodyIn" )) {
        TransactionBodyPtr body;
        serializer.serialize ( "bodyIn", body );
        this->setBody ( body );
    }
    else {
        serializer.serialize ( "body", this->mBodyString );
        if ( this->mBodyString.size ()) {
            FromJSONSerializer::fromJSONString ( this->mBody, this->mBodyString );
            assert ( this->mBody );
        }
    }
}

//----------------------------------------------------------------//
void Transaction::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "signature",     this->mSignature );
    serializer.serialize ( "body",          this->mBodyString );
}

} // namespace Volition
