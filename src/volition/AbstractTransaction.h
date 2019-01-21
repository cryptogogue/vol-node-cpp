// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTTRANSACTION_H
#define VOLITION_ABSTRACTTRANSACTION_H

#include <volition/common.h>
#include <volition/Ledger.h>
#include <volition/TransactionMakerSignature.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

#define TRANSACTION_TYPE(typeString)                            \
    static constexpr const char* TYPE_STRING = typeString;      \
    string AbstractTransaction_typeString () const override {   \
        return TYPE_STRING;                                     \
    }

#define TRANSACTION_WEIGHT(weight)                              \
    static constexpr u64 WEIGHT = weight;                       \
    u64 AbstractTransaction_weight () const override {          \
        return WEIGHT;                                          \
    }

#define TRANSACTION_MATURITY(maturity)                          \
    static constexpr u64 MATURITY = maturity;                   \
    u64 AbstractTransaction_maturity () const override {        \
        return MATURITY;                                        \
    }

//================================================================//
// AbstractTransaction
//================================================================//
class AbstractTransaction :
    public AbstractSerializable {
protected:

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serializeFrom      ( const AbstractSerializerFrom& serializer ) override;
    void                    AbstractSerializable_serializeTo        ( AbstractSerializerTo& serializer ) const override;

    //----------------------------------------------------------------//
    virtual bool            AbstractTransaction_apply               ( Ledger& ledger ) const = 0;
//    virtual bool            AbstractTransaction_checkSignature      ( const Ledger& ledger ) const = 0;
    virtual void            AbstractTransaction_incrementNonce      ( Ledger& ledger ) const = 0;
    virtual u64             AbstractTransaction_maturity            () const = 0;
    virtual string          AbstractTransaction_typeString          () const = 0;
    virtual u64             AbstractTransaction_weight              () const = 0;
    virtual bool            AbstractTransaction_verify              ( const Ledger& ledger ) const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractTransaction                     ();
                            ~AbstractTransaction                    ();
    bool                    apply                                   ( Ledger& ledger ) const;
    u64                     maturity                                () const;
    string                  typeString                              () const;
    u64                     weight                                  () const;
    bool                    verify                                  ( const Ledger& ledger ) const;
};

} // namespace Volition
#endif
