// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_THECONTROLCOMMANDBODYFACTORY_H
#define VOLITION_THECONTROLCOMMANDBODYFACTORY_H

#include <volition/common.h>
#include <volition/AbstractControlCommandBody.h>
#include <volition/Factory.h>
#include <volition/Singleton.h>

namespace Volition {

//================================================================//
// TheControlCommandBodyFactory
//================================================================//
class TheControlCommandBodyFactory :
    public Singleton < TheControlCommandBodyFactory >,
    public Factory < AbstractControlCommandBody > {
public:

    //----------------------------------------------------------------//
    unique_ptr < AbstractControlCommandBody >   create                              ( string typeString ) const;
                                                TheControlCommandBodyFactory        ();
                                                ~TheControlCommandBodyFactory       ();
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void registerTransaction () {
        this->addFactoryAllocator < TYPE >( TYPE::TYPE_STRING );
    }
};

//================================================================//
// ControlCommandBodyFactory
//================================================================//
class ControlCommandBodyFactory :
    public AbstractSerializablePtrFactory < AbstractControlCommandBody > {
public:

    //----------------------------------------------------------------//
    unique_ptr < AbstractControlCommandBody > SerializablePtrFactory_make ( const AbstractSerializerFrom& serializer ) override {
        string type = serializer.serializeIn < string >( "type", "" );
        return TheControlCommandBodyFactory::get ().create ( type );
    }
};

} // namespace Volition
#endif
