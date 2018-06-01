#ifndef VLBLOCK_H
#define VLBLOCK_H

#include "common.h"

#include "AbstractHashable.h"
#include "AbstractSerializable.h"
#include "AbstractTransaction.h"

namespace Volition {

//================================================================//
// Block
//================================================================//
class Block :
    public AbstractHashable,
    public AbstractSerializable {
private:

    friend class Context;

    size_t      mBlockID;
    string      mBlockHash;
    string      mPrevBlockHash;

    vector < unique_ptr < AbstractTransaction >>  mTransactions;

    //----------------------------------------------------------------//
    void        AbstractHashable_hash             ( Poco::DigestOutputStream& digestStream ) const override;
    void        AbstractSerializable_fromJSON     ( const Poco::JSON::Object& object ) override;
    void        AbstractSerializable_toJSON       ( Poco::JSON::Object& object ) const override;

public:

    //----------------------------------------------------------------//
                Block               ();
                ~Block              ();
    void        sign                ();
};

} // namespace Volition
#endif
