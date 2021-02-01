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

    static const size_t MAX_UUID_LENGTH = 36;

    typedef SerializableSharedPtr < AbstractTransactionBody, TransactionBodyFactory > TransactionBodyPtr;
    typedef SerializableSharedPtr < Signature > SignaturePtr;

    TransactionBodyPtr          mBody;          // serialized from/to *body*
    string                      mBodyString;    // store this verbatim
    SignaturePtr                mSignature;     // signatures for *body*
    
    //----------------------------------------------------------------//
    TransactionResult           applyInner              ( Ledger& ledger, SchemaHandle& schemaHandle, time_t time ) const;
    TransactionResult           checkBody               ( Ledger& ledger, time_t time ) const;
    
    //----------------------------------------------------------------//
    void                AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

public:

    //----------------------------------------------------------------//
    TransactionResult           apply                   ( Ledger& ledger, time_t time, SchemaHandle& schemaHandle ) const;
    bool                        checkMaker              ( string accountName, string uuid ) const;
    TransactionResult           checkNonceAndSignature  ( const Ledger& ledger, const Account& account, const KeyAndPolicy& keyAndPolicy ) const;
    TransactionResult           control                 ( Miner& miner ) const;
    u64                         getGratuity             () const;
    const TransactionMaker*     getMaker                () const;
    u64                         getNonce                () const;
    string                      getUUID                 () const;
    u64                         maturity                () const;
    bool                        needsControl            () const;
    void                        setBody                 ( shared_ptr < AbstractTransactionBody > body );
                                Transaction             ();
                                ~Transaction            ();
    string                      typeString              () const;
    u64                         weight                  () const;
};

} // namespace Volition
#endif
