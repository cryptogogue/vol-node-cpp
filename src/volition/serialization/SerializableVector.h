// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_SERIALIZABLEVECTOR_H
#define VOLITION_SERIALIZATION_SERIALIZABLEVECTOR_H

#include <volition/serialization/AbstractSerializerFrom.h>
#include <volition/serialization/AbstractSerializerTo.h>

namespace Volition {

//================================================================//
// SerializableVector
//================================================================//
template < typename TYPE >
class SerializableVector :
    public AbstractSerializable,
    public vector < TYPE > {
public:
    
    //----------------------------------------------------------------//
    SerializableVector& operator = ( const vector < TYPE >& other ) {
        vector < TYPE >::operator = ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    SerializableVector& operator = ( const SerializableVector < TYPE >& other ) {
        vector < TYPE >::operator = ( other );
        return *this;
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        this->clear ();
    
        if ( serializer.getKeyType () == AbstractSerializerFrom::KEY_TYPE_INDEX ) {
        
            size_t size = serializer.getSize ();
            this->resize ( size );
            
            for ( size_t i = 0; i < size; ++i ) {
                serializer.serialize ( i, this->data ()[ i ]);
            }
        }
        else {

            const AbstractSerializerFrom* parent = serializer.getParent ();
            if ( parent ) {
                this->resize ( 1 );
                parent->serialize ( serializer.getName (), this->data ()[ 0 ]);
            }
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
    
        serializer.affirmArray ();
    
        size_t size = this->size ();
        for ( size_t i = 0; i < size; ++i ) {
            serializer.serialize ( i, ( *this )[ i ]);
        }
    }
    
    //----------------------------------------------------------------//
    SerializableVector () {
    }
    
    //----------------------------------------------------------------//
    SerializableVector ( const vector < TYPE >& other ) :
        vector < TYPE >( other ) {
    }
    
    //----------------------------------------------------------------//
    SerializableVector ( const SerializableVector < TYPE >& other ) :
        vector < TYPE >( other ) {
    }
};

} // namespace Volition
#endif
