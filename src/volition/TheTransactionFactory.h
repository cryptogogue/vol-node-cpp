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
    AbstractTransaction*    create                      ( const Poco::JSON::Object& object ) const;
                            TheTransactionFactory       ();
                            ~TheTransactionFactory      ();
    
    //----------------------------------------------------------------//
    template < typename TYPE >
    void registerTransaction () {
        this->addFactoryAllocator < TYPE >( TYPE::TYPE_STRING );
    }
};

} // namespace Volition
#endif
