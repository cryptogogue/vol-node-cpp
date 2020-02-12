// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THETRANSACTIONBODYFACTORY_H
#define VOLITION_THETRANSACTIONBODYFACTORY_H

#include <volition/common.h>
#include <volition/AbstractTransactionBody.h>
#include <volition/Factory.h>
#include <volition/Singleton.h>

namespace Volition {

//================================================================//
// TheTransactionBodyFactory
//================================================================//
class TheTransactionBodyFactory :
    public Singleton < TheTransactionBodyFactory >,
    public Factory < AbstractTransactionBody > {
public:

    //----------------------------------------------------------------//
    unique_ptr < AbstractTransactionBody >      create                      ( string typeString ) const;
                                            TheTransactionBodyFactory       ();
                                            ~TheTransactionBodyFactory      ();
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void registerTransaction () {
        this->addFactoryAllocator < TYPE >( TYPE::TYPE_STRING );
    }
};

//================================================================//
// TransactionBodyFactory
//================================================================//
class TransactionBodyFactory :
    public AbstractSerializablePtrFactory < AbstractTransactionBody > {
public:

    //----------------------------------------------------------------//
    unique_ptr < AbstractTransactionBody > SerializablePtrFactory_make ( const AbstractSerializerFrom& serializer ) override {
        string type = serializer.serializeIn < string >( "type", "" );
        return TheTransactionBodyFactory::get ().create ( type );
    }
};

} // namespace Volition
#endif
