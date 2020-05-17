// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTTRANSACTIONBODY_H
#define VOLITION_ABSTRACTTRANSACTIONBODY_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/serialization/Serialization.h>
#include <volition/TransactionContext.h>
#include <volition/TransactionMaker.h>
#include <volition/TransactionResult.h>

namespace Volition {

#define TRANSACTION_TYPE(typeString)                                \
    static constexpr const char* TYPE_STRING = typeString;          \
    string AbstractTransactionBody_typeString () const override {   \
        return TYPE_STRING;                                         \
    }

#define TRANSACTION_WEIGHT(weight)                                  \
    static constexpr u64 WEIGHT = weight;                           \
    u64 AbstractTransactionBody_weight () const override {          \
        return WEIGHT;                                              \
    }

#define TRANSACTION_MATURITY(maturity)                              \
    static constexpr u64 MATURITY = maturity;                       \
    u64 AbstractTransactionBody_maturity () const override {        \
        return MATURITY;                                            \
    }

class Transaction;

//================================================================//
// AbstractTransactionBody
//================================================================//
class AbstractTransactionBody :
    public AbstractSerializable {
protected:

    friend class Transaction;

    SerializableUniquePtr < TransactionMaker >      mMaker;
    u64                                             mMaxHeight; // expiration block
    SerializableTime                                mRecordBy; // expiration date/time
    string                                          mNote;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

    //----------------------------------------------------------------//
    virtual TransactionResult   AbstractTransactionBody_apply           ( TransactionContext& context ) const = 0;
    virtual u64                 AbstractTransactionBody_cost            () const;
    virtual u64                 AbstractTransactionBody_maturity        () const = 0;
    virtual string              AbstractTransactionBody_typeString      () const = 0;
    virtual u64                 AbstractTransactionBody_weight          () const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractTransactionBody                 ();
                            ~AbstractTransactionBody                ();
    TransactionResult       apply                                   ( TransactionContext& context ) const;
    u64                     cost                                    () const;
    u64                     gratuity                                () const;
    u64                     maturity                                () const;
    string                  note                                    () const;
    string                  typeString                              () const;
    u64                     weight                                  () const;
};

} // namespace Volition
#endif
