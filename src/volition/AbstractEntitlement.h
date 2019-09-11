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
    const AbstractEntitlement*              resolvePath                                 ( string path ) const;

    //----------------------------------------------------------------//
    virtual bool                            AbstractEntitlement_check                   () const;
    virtual bool                            AbstractEntitlement_check                   ( double value ) const;
//    virtual const AbstractEntitlement*      AbstractEntitlement_getChild                ( string name ) const;
    virtual bool                            AbstractEntitlement_isMatchOrSubsetOf       ( const AbstractEntitlement* abstractOther ) const = 0;

public:

    //----------------------------------------------------------------//
                                    AbstractEntitlement             ();
                                    ~AbstractEntitlement            ();
    bool                            check                           () const;
    bool                            check                           ( string path ) const;
    const AbstractEntitlement*      getChild                        ( string name ) const;
    bool                            isMatchOrSubsetOf               ( const AbstractEntitlement* abstractOther ) const;

    //----------------------------------------------------------------//
    template < typename TYPE >
    bool check ( string path, TYPE value ) const {
    
        const AbstractEntitlement* leaf = this->resolvePath ( path );
        if ( leaf ) {
            return leaf->AbstractEntitlement_check ( value );
        }
        return false;
    }
};

} // namespace Volition
#endif
