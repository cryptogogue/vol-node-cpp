#ifndef VOLITION_BLOCK_H
#define VOLITION_BLOCK_H

#include "common.h"

#include "AbstractSerializable.h"
#include "AbstractTransaction.h"
#include "Signable.h"

namespace Volition {

//================================================================//
// Block
//================================================================//
class Block :
    public AbstractSerializable,
    public Signable {
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
};

} // namespace Volition
#endif
