// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/AbstractTransactionBody.h>
#include <volition/Miner.h>
#include <volition/Transaction.h>

namespace Volition {

static const size_t MAX_UUID_LENGTH = 36;

//================================================================//
// AbstractTransactionBody
//================================================================//

//----------------------------------------------------------------//
AbstractTransactionBody::AbstractTransactionBody () {
}

//----------------------------------------------------------------//
AbstractTransactionBody::~AbstractTransactionBody () {
}

//----------------------------------------------------------------//
TransactionResult AbstractTransactionBody::apply ( TransactionContext& context ) const {
    
    TransactionResult result = this->AbstractTransactionBody_preApply ( context );
    if ( !result ) return result;
    
    result = this->AbstractTransactionBody_apply ( context );
    if ( !result ) return result;
    
    return this->AbstractTransactionBody_postApply ( context );
}

//----------------------------------------------------------------//
TransactionDetailsPtr AbstractTransactionBody::getDetails ( const AbstractLedger& ledger ) const {
    return this->AbstractTransactionBody_getDetails ( ledger );
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::getVOL ( const TransactionContext& context ) const {
    return AbstractTransactionBody_getVOL ( context );
}

//----------------------------------------------------------------//
void AbstractTransactionBody::setMaker ( const TransactionMaker& maker ) {
    this->mMaker = maker;
}

//----------------------------------------------------------------//
bool AbstractTransactionBody::wasApplied ( const AbstractLedger& ledger ) const {
    return AbstractTransactionBody_wasApplied ( ledger );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
void AbstractTransactionBody::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

    string type;
    
    serializer.serialize ( "type",      type );
    serializer.serialize ( "maker",     this->mMaker );
    serializer.serialize ( "maxHeight", this->mMaxHeight );
    serializer.serialize ( "recordBy",  this->mRecordBy );
    serializer.serialize ( "uuid",      this->mUUID );
    
    assert ( type == this->getTypeString ());
}

//----------------------------------------------------------------//
void AbstractTransactionBody::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

    serializer.serialize ( "type",      this->getTypeString ());
    serializer.serialize ( "maker",     this->mMaker );
    serializer.serialize ( "maxHeight", this->mMaxHeight );
    serializer.serialize ( "recordBy",  this->mRecordBy );
    serializer.serialize ( "uuid",      this->mUUID );
}

//----------------------------------------------------------------//
TransactionResult AbstractTransactionBody::AbstractTransactionBody_apply ( TransactionContext& context ) const {
    UNUSED ( context );
    return true;
}

//----------------------------------------------------------------//
TransactionDetailsPtr AbstractTransactionBody::AbstractTransactionBody_getDetails ( const AbstractLedger& ledger ) const {
    UNUSED ( ledger );
    return NULL;
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::AbstractTransactionBody_getVOL ( const TransactionContext& context ) const {
    UNUSED ( context );
    return 0;
}

//----------------------------------------------------------------//
u64 AbstractTransactionBody::AbstractTransactionBody_minRelease () const {
    return 0;
}

//----------------------------------------------------------------//
TransactionResult AbstractTransactionBody::AbstractTransactionBody_postApply ( TransactionContext& context ) const {
    
    if ( !( context.isGenesis () || context.isProvisional ())) {
        
        context.mAccountODBM.incAccountTransactionNonce ( this->getNonce (), this->getUUID ());
        context.pushAccountLogEntry ();
        context.mAccountODBM.subFunds ( this->getFees ());
    }
    return true;
}

//----------------------------------------------------------------//
TransactionResult AbstractTransactionBody::AbstractTransactionBody_preApply ( TransactionContext& context ) const {

    if ( this->mUUID.size () > MAX_UUID_LENGTH ) return Format::write ( "Transaction UUID exceeds %d-character limit.", MAX_UUID_LENGTH );

    if ( this->mRecordBy > 0 ) {
        if ( context.mTime > this->mRecordBy ) return Format::write ( "Transaction expired at %s.", (( string )this->mRecordBy ).c_str ());
    }

    AbstractLedger& ledger = context.mLedger;

    if ( context.mRelease < this->AbstractTransactionBody_minRelease ()) return "Unsupported transaction type for currently accepted release.";

    if ( this->mMaxHeight > 0 ) {
        u64 height = ledger.getHeight ();
        if ( height > this->mMaxHeight ) return Format::write ( "Transaction expired at chain height of %d.", this->mMaxHeight );
    }
    
    if ( ledger.isGenesis ()) return true;
    
    const TransactionFeeProfile& feeProfile = context.mFeeSchedule.getFeeProfile ( this->getTypeString ());
        
    if ( !feeProfile.checkProfitShare ( context.mMaker.getGratuity (), context.mMaker.getProfitShare ())) return "Incorrect profit share.";
    if ( !feeProfile.checkTransferTax ( this->getVOL ( context ), context.mMaker.getTransferTax ())) return "Incorrect transfer tax.";
    
    if ( context.isProvisional ()) return true;
    
    if ( !context.mAccountODBM ) return "Transaction maker account not found.";
    if ( !context.mKeyAndPolicy ) return "Transaction maker key not found.";
    
    u64 fees = this->getFees ();
    if ( !context.mAccountODBM.hasFunds ( fees + this->getVOL ( context ))) return "Insufficient funds.";
        
    u64 nonce = context.mAccountODBM.mTransactionNonce.get ();
    if ( nonce != this->getNonce ()) return "Mismatched trabsaction nonce.";
    
    return true;
}

//----------------------------------------------------------------//
bool AbstractTransactionBody::AbstractTransactionBody_wasApplied ( const AbstractLedger& ledger ) const {
    UNUSED ( ledger );
    return true;
}

} // namespace Volition
