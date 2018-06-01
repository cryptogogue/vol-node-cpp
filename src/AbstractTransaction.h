#ifndef VLABSTRACTTRANSACTION_H
#define VLABSTRACTTRANSACTION_H

#include "common.h"

#include "AbstractHashable.h"
#include "AbstractSerializable.h"

namespace Volition {

#define TRANSACTION_TYPE ( typeString )                         \
    char const* VLAbstractTransaction_typeString () const {     \
        return typeString;                                      \
    }

#define TRANSACTION_WEIGHT ( weight )                           \
    size_t VLAbstractTransaction_weight () const {              \
        return weight;                                          \
    }

//================================================================//
// AbstractTransaction
//================================================================//
class AbstractTransaction :
    public AbstractHashable,
    public AbstractSerializable {
private:

    //----------------------------------------------------------------//
    void                    AbstractHashable_hash               ( Poco::DigestOutputStream& digestStream ) const override;
    void                    AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) override;
    void                    AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const override;

    //----------------------------------------------------------------//
    virtual char const*     AbstractTransaction_typeString      () const = 0;
    virtual size_t          AbstractTransaction_weight          () const = 0;

public:

    //----------------------------------------------------------------//
                            AbstractTransaction                 ();
                            ~AbstractTransaction                ();
};

} // namespace Volition
#endif
