// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_FACTORY_H
#define VOLITION_FACTORY_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AbstractFactoryAllocator
//================================================================//
template < typename FACTORY_TYPE >
class AbstractFactoryAllocator {
protected:

    //----------------------------------------------------------------//
    virtual unique_ptr < FACTORY_TYPE >     AbstractFactoryAllocator_create     () const = 0;

public:

    //----------------------------------------------------------------//
    unique_ptr < FACTORY_TYPE > create () const {
    
        return this->AbstractFactoryAllocator_create ();
    }
};

//================================================================//
// FactoryAllocator
//================================================================//
template < typename FACTORY_TYPE, typename PRODUCT_TYPE >
class FactoryAllocator :
    public AbstractFactoryAllocator < FACTORY_TYPE > {
private:

    //----------------------------------------------------------------//
    unique_ptr < FACTORY_TYPE > AbstractFactoryAllocator_create () const override {
        return make_unique < PRODUCT_TYPE >();
    }
};

//================================================================//
// Factory
//================================================================//
template < typename FACTORY_TYPE, typename KEY_TYPE = string >
class Factory {
private:

    map < KEY_TYPE, unique_ptr < AbstractFactoryAllocator < FACTORY_TYPE >>>    mKeysToAllocators;
    unique_ptr < AbstractFactoryAllocator < FACTORY_TYPE >>                     mDefaultAllocator;

protected:

    //----------------------------------------------------------------//
    template < typename PRODUCT_TYPE >
    void addFactoryAllocator ( KEY_TYPE key ) {
        this->mKeysToAllocators [ key ] = make_unique < FactoryAllocator < FACTORY_TYPE, PRODUCT_TYPE >> ();
    }

    //----------------------------------------------------------------//
    virtual unique_ptr < FACTORY_TYPE > create () const {

        return this->mDefaultAllocator ? this->mDefaultAllocator->create () : NULL;
    }

    //----------------------------------------------------------------//
    virtual unique_ptr < FACTORY_TYPE > create ( KEY_TYPE key ) const {
        
        typename map < KEY_TYPE, unique_ptr < AbstractFactoryAllocator < FACTORY_TYPE >>>::const_iterator allocatorIt = this->mKeysToAllocators.find ( key );

        if ( allocatorIt != this->mKeysToAllocators.cend ()) {
            return allocatorIt->second->create ();
        }
        return this->create ();
    }
    
    //----------------------------------------------------------------//
    Factory () {
    }
    
    //----------------------------------------------------------------//
    virtual ~Factory () {
    }
    
    //----------------------------------------------------------------//
    size_t getFactorySize () const {
    
        return this->mKeysToAllocators.size ();
    }
    
    //----------------------------------------------------------------//
    template < typename PRODUCT_TYPE >
    void setDefaultFactoryAllocator () {
        this->mDefaultAllocator = make_unique < FactoryAllocator < FACTORY_TYPE, PRODUCT_TYPE >> ();
    }
};

} // namespace Volition
#endif
