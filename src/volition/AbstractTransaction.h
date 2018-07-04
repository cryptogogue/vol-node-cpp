// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTTRANSACTION_H
#define VOLITION_ABSTRACTTRANSACTION_H

#include <volition/common.h>
#include <volition/AbstractHashable.h>
#include <volition/AbstractSerializable.h>
#include <volition/State.h>
#include <volition/TransactionMakerSignature.h>

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
    public AbstractHashable,
    public AbstractSerializable {
protected:

    unique_ptr < TransactionMakerSignature >    mMakerSignature;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

    //----------------------------------------------------------------//
    virtual void            AbstractTransaction_apply           ( State& state ) const = 0;
    virtual string          AbstractTransaction_typeString      () const = 0;
    virtual size_t          AbstractTransaction_weight          () const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractTransaction                 ();
                            ~AbstractTransaction                ();
    void                    apply                               ( State& state ) const;
    string                  typeString                          () const;
    size_t                  weight                              () const;
};

} // namespace Volition
#endif
