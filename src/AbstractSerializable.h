#ifndef VLABSTRACTSERIALIZABLE_H
#define VLABSTRACTSERIALIZABLE_H

#include "common.h"

namespace Volition {

//================================================================//
// AbstractSerializable
//================================================================//
class AbstractSerializable {
private:

    //----------------------------------------------------------------//
    virtual void        AbstractSerializable_fromJSON       ( const Poco::JSON::Object& object ) = 0;
    virtual void        AbstractSerializable_toJSON         ( Poco::JSON::Object& object ) const = 0;

public:

    //----------------------------------------------------------------//
    AbstractSerializable () {
    }
    
    //----------------------------------------------------------------//
    virtual ~AbstractSerializable () {
    }

    //----------------------------------------------------------------//
    void fromJSON ( const Poco::JSON::Object& object ) {
        this->AbstractSerializable_fromJSON ( object );
    }
    
    //----------------------------------------------------------------//
    void toJSON ( Poco::JSON::Object& object ) const {
        this->AbstractSerializable_toJSON ( object );
    }
};

} // namespace Volition
#endif
