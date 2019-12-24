// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEOPAQUE_H
#define VOLITION_SERIALIZATION_SERIALIZABLEOPAQUE_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// SerializableOpaque
//================================================================//
class SerializableOpaque :
    public AbstractSerializable {
public:
    
    // TODO: need to handle all serializer types
    string      mString;
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
        
        serializer.getParent ()->stringFromTree ( serializer.getName (), this->mString );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
        
        serializer.getParent ()->stringToTree ( serializer.getName (), this->mString );
    }
};

} // namespace Volition
#endif
