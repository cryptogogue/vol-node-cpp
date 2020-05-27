// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_TRANSACTION_H
#define VOLITION_TRANSACTION_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Ledger.h>
#include <volition/serialization/Serialization.h>
#include <volition/Signature.h>
#include <volition/TheTransactionBodyFactory.h>

namespace Volition {

class TransactionMaker;

//================================================================//
// Transaction
//================================================================//
class Transaction :
    public AbstractSerializable {
protected:

    static const size_t MAX_NOTE_LENGTH = 64;

    typedef SerializableSharedPtr < AbstractTransactionBody, TransactionBodyFactory > TransactionBodyPtr;
    typedef SerializableSharedPtr < Signature > SignaturePtr;

    TransactionBodyPtr      mBody;          // serialized from/to *body*
    string                  mBodyString;    // store this verbatim
    SignaturePtr            mSignature;     // signatures for *body*
    
    //----------------------------------------------------------------//
    TransactionResult           applyInner              ( Ledger& ledger, SchemaHandle& schemaHandle ) const;
    TransactionResult           checkBody               ( Ledger& ledger, time_t time ) const;
    
    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    TransactionResult           apply                   ( Ledger& ledger, time_t time, SchemaHandle& schemaHandle ) const;
    bool                        checkMaker              ( string accountName, u64 nonce ) const;
    TransactionResult           checkNonceAndSignature  ( const Ledger& ledger, const Account& account, const KeyAndPolicy& keyAndPolicy ) const;
    TransactionResult           control                 ( Miner& miner, Ledger& ledger ) const;
    u64                         getGratuity             () const;
    const TransactionMaker*     getMaker                () const;
    string                      getNote                 () const;
    u64                         maturity                () const;
    void                        setBody                 ( shared_ptr < AbstractTransactionBody > body );
                                Transaction             ();
                                ~Transaction            ();
    u64                         weight                  () const;
};

} // namespace Volition
#endif
