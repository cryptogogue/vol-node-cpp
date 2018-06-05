#ifndef VOLITION_MINERSINGLETON_H
#define VOLITION_MINERSINGLETON_H

#include "common.h"
#include "AbstractTransaction.h"
#include "Factory.h"
#include "Singleton.h"

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
