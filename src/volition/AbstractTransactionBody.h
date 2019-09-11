// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTTRANSACTIONBODY_H
#define VOLITION_ABSTRACTTRANSACTIONBODY_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/serialization/Serialization.h>
#include <volition/TransactionMaker.h>

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

    SerializableUniquePtr < TransactionMaker >  mMaker;
    u64                                         mMaxHeight; // expiration block

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

    //----------------------------------------------------------------//
    virtual bool            AbstractTransactionBody_apply           ( Ledger& ledger ) const = 0;
    virtual u64             AbstractTransactionBody_maturity        () const = 0;
    virtual string          AbstractTransactionBody_typeString      () const = 0;
    virtual u64             AbstractTransactionBody_weight          () const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractTransactionBody                 ();
                            ~AbstractTransactionBody                ();
    bool                    apply                                   ( Ledger& ledger ) const;
    u64                     maturity                                () const;
    string                  typeString                              () const;
    u64                     weight                                  () const;
};

} // namespace Volition
#endif
