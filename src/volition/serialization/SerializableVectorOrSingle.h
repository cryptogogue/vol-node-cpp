// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEVECTORORSINGLE_H
#define VOLITION_SERIALIZATION_SERIALIZABLEVECTORORSINGLE_H

#include <volition/serialization/SerializableVector.h>

namespace Volition {

//================================================================//
// SerializableVector
//================================================================//
template < typename TYPE >
class SerializableVectorOrSingle :
    public SerializableVector < TYPE > {
public:
    
    //----------------------------------------------------------------//
    SerializableVectorOrSingle& operator = ( const vector < TYPE >& other ) {
        SerializableVector < TYPE >::operator = ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    SerializableVectorOrSingle& operator = ( const SerializableVector < TYPE >& other ) {
        SerializableVector < TYPE >::operator = ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    SerializableVectorOrSingle& operator = ( const SerializableVectorOrSingle < TYPE >& other ) {
        SerializableVector < TYPE >::operator = ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.affirmArray ();
    
        size_t size = this->size ();
        if ( size == 1 ) {
            ( *this )[ 0 ].serializeTo ( serializer );
        }
        else {
            for ( size_t i = 0; i < size; ++i ) {
                serializer.serialize ( i, ( *this )[ i ]);
            }
        }
    }
    
    //----------------------------------------------------------------//
    SerializableVectorOrSingle () {
    }
    
    //----------------------------------------------------------------//
    SerializableVectorOrSingle ( const vector < TYPE >& other ) :
        vector < TYPE >( other ) {
    }
    
    //----------------------------------------------------------------//
    SerializableVectorOrSingle ( const SerializableVector < TYPE >& other ) :
        vector < TYPE >( other ) {
    }
    
    //----------------------------------------------------------------//
    SerializableVectorOrSingle ( const SerializableVectorOrSingle < TYPE >& other ) :
        vector < TYPE >( other ) {
    }
};

} // namespace Volition
#endif
