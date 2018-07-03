// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTSERIALIZABLE_H
#define VOLITION_ABSTRACTSERIALIZABLE_H

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
    void fromJSON ( istream& inStream ) {
    
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse ( inStream );
        Poco::JSON::Object::Ptr object = result.extract < Poco::JSON::Object::Ptr >();
        
        this->fromJSON ( *object );
    }
    
    //----------------------------------------------------------------//
    void fromJSON ( string json ) {
        
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse ( json );
        Poco::JSON::Object::Ptr object = result.extract < Poco::JSON::Object::Ptr >();
        
        this->fromJSON ( *object );
    }
    
    //----------------------------------------------------------------//
    Poco::JSON::Object::Ptr toJSON () const {
    
        Poco::JSON::Object::Ptr object = new Poco::JSON::Object ();
        this->toJSON ( *object );
        return object;
    }
    
    //----------------------------------------------------------------//
    void toJSON ( Poco::JSON::Object& object ) const {
    
        this->AbstractSerializable_toJSON ( object );
    }
    
    //----------------------------------------------------------------//
    void toJSON ( ostream& outStream, unsigned int indent = 4, int step = -1 ) const {
    
        Poco::JSON::Object::Ptr object = this->toJSON ();
        object->stringify ( outStream, indent, step );
    }
};

} // namespace Volition
#endif
