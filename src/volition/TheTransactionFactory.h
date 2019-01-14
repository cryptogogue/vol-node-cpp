// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_MINERSINGLETON_H
#define VOLITION_MINERSINGLETON_H

#include <volition/common.h>
#include <volition/AbstractTransaction.h>
#include <volition/Factory.h>
#include <volition/Singleton.h>

namespace Volition {

//================================================================//
// TheTransactionFactory
//================================================================//
class TheTransactionFactory :
    public Singleton < TheTransactionFactory >,
    public Factory < AbstractTransaction > {
public:

    //----------------------------------------------------------------//
    unique_ptr < AbstractTransaction >      create                      ( string typeString ) const;
                                            TheTransactionFactory       ();
                                            ~TheTransactionFactory      ();
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void registerTransaction () {
        this->addFactoryAllocator < TYPE >( TYPE::TYPE_STRING );
    }
};

//================================================================//
// TransactionFactory
//================================================================//
class TransactionFactory :
    public AbstractSerializablePtrFactory < AbstractTransaction > {
public:

    //----------------------------------------------------------------//
    unique_ptr < AbstractTransaction > SerializablePtrFactory_make ( const AbstractSerializerFrom& serializer ) override {
        string type = serializer.serializeIn < string >( "type", "" );
        return TheTransactionFactory::get ().create ( type );
    }
};

} // namespace Volition
#endif
