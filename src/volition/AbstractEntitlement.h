// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTENTITLEMENT_H
#define VOLITION_ABSTRACTENTITLEMENT_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AbstractEntitlement
//================================================================//
class AbstractEntitlement :
    public AbstractSerializable {
public:

    static constexpr const char* ENTITLEMENT_TYPE_BOOLEAN           = "boolean";
    static constexpr const char* ENTITLEMENT_TYPE_NUMERIC           = "numeric";
    static constexpr const char* ENTITLEMENT_TYPE_PATH              = "path";
    
protected:

    //----------------------------------------------------------------//
    virtual bool        AbstractEntitlement_check                   () const;
    virtual bool        AbstractEntitlement_check                   ( double value ) const;
    virtual bool        AbstractEntitlement_isMatchOrSubsetOf       ( const AbstractEntitlement* abstractOther ) const = 0;

public:

    //----------------------------------------------------------------//
                        AbstractEntitlement             ();
                        ~AbstractEntitlement            ();
    bool                check                           () const;
    bool                isMatchOrSubsetOf               ( const AbstractEntitlement* abstractOther ) const;

    //----------------------------------------------------------------//
    template < typename TYPE >
    bool check ( TYPE value ) const {
        return this->AbstractEntitlement_check ( value );
    }
};

} // namespace Volition
#endif
