#ifndef VLABSTRACTHASHABLE_H
#define VLABSTRACTHASHABLE_H

#include "common.h"

namespace Volition {

//================================================================//
// AbstractHashable
//================================================================//
class AbstractHashable {
private:

    //----------------------------------------------------------------//
    virtual void            AbstractHashable_hash            ( Poco::DigestOutputStream& digestStream ) const = 0;

public:

    //----------------------------------------------------------------//
    AbstractHashable () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractHashable () {
    }

    //----------------------------------------------------------------//
    void hash ( Poco::DigestOutputStream& digestStream ) const {
        this->AbstractHashable_hash ( digestStream );
    }
};

} // namespace Volition
#endif
