#ifndef VOLITION_ABSTRACTTRANSACTION_H
#define VOLITION_ABSTRACTTRANSACTION_H

#include "common.h"

#include "AbstractSerializable.h"
#include "Signable.h"

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
    public AbstractSerializable,
    public Signable {
protected:

    size_t                  mTransactionID;
    size_t                  mGratuity;

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

    //----------------------------------------------------------------//
    virtual string          AbstractTransaction_typeString      () const = 0;
    virtual size_t          AbstractTransaction_weight          () const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractTransaction                 ();
                            ~AbstractTransaction                ();
    string                  typeString                          ();
    size_t                  weight                              ();
};

} // namespace Volition
#endif
