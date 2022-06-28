// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_H
#define VOLITION_TRANSACTION_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Block.h>
#include <volition/serialization/Serialization.h>
#include <volition/Signature.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {

class Ledger;
class Miner;

//================================================================//
// Transaction
//================================================================//
class Transaction :
    public AbstractSerializable {
protected:

    static const size_t MAX_UUID_LENGTH = 36;

    typedef SerializableSharedPtr < AbstractTransactionBody, TransactionBodyFactory > TransactionBodyPtr;
    typedef SerializableSharedPtr < Signature > SignaturePtr;

    TransactionBodyPtr          mBody;          // serialized from/to *body*
    string                      mBodyString;    // store this verbatim
    SignaturePtr                mSignature;     // signatures for *body*
    
    //----------------------------------------------------------------//
    TransactionResult           checkSignature              ( const TransactionContext& context, Block::VerificationPolicy policy ) const;
    
    //----------------------------------------------------------------//
    void                        AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                        AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    GET ( u64,                          Fees,                       this->mBody->getFees ())
    GET ( u64,                          Gratuity,                   this->mBody->getGratuity ())
    GET ( const TransactionMaker&,      Maker,                      this->mBody->getMaker ())
    GET ( string,                       MakerAccountName,           this->mBody->getMakerAccountName ())
    GET ( u64,                          Maturity,                   this->mBody->getMaturity ())
    GET ( u64,                          Nonce,                      this->mBody->getNonce ())
    GET ( u64,                          ProfitShare,                this->mBody->getProfitShare ())
    GET ( u64,                          TransferTax,                this->mBody->getTransferTax ())
    GET ( string,                       TypeString,                 this->mBody->getTypeString ())
    GET ( string,                       UUID,                       this->mBody->getUUID ())
    GET ( u64,                          Weight,                     this->mBody->getWeight ())
    
    //----------------------------------------------------------------//
    TransactionResult           apply                       ( AbstractLedger& ledger, u64 blockHeight, u64 index, time_t time, Block::VerificationPolicy policy ) const;
    bool                        checkMaker                  ( string accountName, string uuid ) const;
    TransactionDetailsPtr       getDetails                  ( const AbstractLedger& ledger ) const;
    void                        setBody                     ( shared_ptr < AbstractTransactionBody > body );
    void                        sign                        ( const CryptoKeyPair& keyPair );
                                Transaction                 ();
                                ~Transaction                ();
    bool                        wasApplied                  ( const AbstractLedger& ledger ) const;
};

} // namespace Volition
#endif
