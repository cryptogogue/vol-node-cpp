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
    TransactionResult           applyInner                  ( AbstractLedger& ledger, u64 blockHeight, u64 release, u64 index, time_t time, Block::VerificationPolicy policy ) const;
    TransactionResult           checkBody                   ( AbstractLedger& ledger, time_t time ) const;
    
    //----------------------------------------------------------------//
    void                        AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                        AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    GET_COMPOSED ( u64,                          Fees,                      this->mBody,        0 )
    GET_COMPOSED ( u64,                          Gratuity,                  this->mBody,        0 )
    GET_COMPOSED ( const TransactionMaker*,      Maker,                     this->mBody,        NULL )
    GET_COMPOSED ( u64,                          Maturity,                  this->mBody,        0 )
    GET_COMPOSED ( u64,                          Nonce,                     this->mBody,        0 )
    GET_COMPOSED ( u64,                          ProfitShare,               this->mBody,        0 )
    GET_COMPOSED ( u64,                          TransferTax,               this->mBody,        0 )
    GET_COMPOSED ( string,                       TypeString,                this->mBody,        "" )
    GET_COMPOSED ( string,                       UUID,                      this->mBody,        "" )
    GET_COMPOSED ( u64,                          Weight,                    this->mBody,        0 )

    //----------------------------------------------------------------//
    TransactionResult           apply                       ( AbstractLedger& ledger, u64 blockHeight, u64 release, u64 index, time_t time, Block::VerificationPolicy policy ) const;
    bool                        checkMaker                  ( string accountName, string uuid ) const;
    TransactionResult           checkNonceAndSignature      ( const AbstractLedger& ledger, AccountID accountID, const CryptoPublicKey& key, Block::VerificationPolicy policy ) const;
    TransactionDetailsPtr       getDetails                  ( const AbstractLedger& ledger ) const;
    void                        setBody                     ( shared_ptr < AbstractTransactionBody > body );
    void                        sign                        ( const CryptoKeyPair& keyPair );
                                Transaction                 ();
                                ~Transaction                ();
};

} // namespace Volition
#endif
