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

    enum Leniency {
        IS_MORE_RESTRICTIVE         = -1,
        IS_EQUALLY_RESTRICTIVE      = 0,
        IS_LESS_RESTRICTIVE         = 1,
    };

    enum Range {
        ALWAYS_FALSE                = 0,
        ALWAYS_TRUE                 = 1,
        MAY_BE_TRUE_OR_FALSE        = 2,
    };

    static constexpr const char* ENTITLEMENT_TYPE_BOOLEAN   = "boolean";
    static constexpr const char* ENTITLEMENT_TYPE_NUMERIC   = "numeric";
    static constexpr const char* ENTITLEMENT_TYPE_PATH      = "path";
    
protected:

    friend class PathEntitlement;

    //----------------------------------------------------------------//
    Leniency            compare                     ( const AbstractEntitlement* abstractOther ) const;
    static Leniency     compareRanges               ( Range r0, Range r1 );
    static Leniency     compareRanges               ( const AbstractEntitlement* e0, const AbstractEntitlement* e1 );
    Range               getRange                    () const;

    //----------------------------------------------------------------//
    virtual bool                                    AbstractEntitlement_check           () const = 0;
    virtual bool                                    AbstractEntitlement_check           ( double value ) const = 0;
    virtual shared_ptr < AbstractEntitlement >      AbstractEntitlement_clone           () const = 0;
    virtual Leniency                                AbstractEntitlement_compare         ( const AbstractEntitlement* abstractOther ) const = 0;
    virtual Range                                   AbstractEntitlement_getRange        () const = 0;
    virtual void                                    AbstractEntitlement_print           () const = 0;

public:

    //----------------------------------------------------------------//
                                            AbstractEntitlement         ();
                                            ~AbstractEntitlement        ();
    bool                                    check                       () const;
    shared_ptr < AbstractEntitlement >      clone                       () const;
    bool                                    isMatchOrSubsetOf           ( const AbstractEntitlement* abstractOther ) const;
    void                                    print                       () const;

    //----------------------------------------------------------------//
    template < typename TYPE >
    bool check ( TYPE value ) const {
        return this->AbstractEntitlement_check ( value );
    }
};

} // namespace Volition
#endif
