// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTTRANSACTION_H
#define VOLITION_ABSTRACTTRANSACTION_H

#include <volition/common.h>
#include <volition/State.h>
#include <volition/TransactionMakerSignature.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

#define TRANSACTION_TYPE(typeString)                            \
    static constexpr const char* TYPE_STRING = typeString;      \
    string AbstractTransaction_typeString () const override {   \
        return TYPE_STRING;                                     \
    }

#define TRANSACTION_WEIGHT(weight)                              \
    static constexpr size_t WEIGHT = weight;                    \
    size_t AbstractTransaction_weight () const override {       \
        return WEIGHT;                                          \
    }

//================================================================//
// AbstractTransaction
//================================================================//
class AbstractTransaction :
    public AbstractSerializable {
protected:

    SerializableUniquePtr < TransactionMakerSignature >     mMakerSignature;

    //----------------------------------------------------------------//
    void                    AbstractSerializable_serialize      ( AbstractSerializer& serializer ) override;

    //----------------------------------------------------------------//
    virtual bool            AbstractTransaction_apply           ( State& state ) const = 0;
    virtual string          AbstractTransaction_typeString      () const = 0;
    virtual size_t          AbstractTransaction_weight          () const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractTransaction                 ();
                            ~AbstractTransaction                ();
    bool                    apply                               ( State& state ) const;
    string                  typeString                          () const;
    size_t                  weight                              () const;
};

} // namespace Volition
#endif
