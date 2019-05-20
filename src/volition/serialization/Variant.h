// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SERIALIZATION_VARIANTL_H
#define VOLITION_SERIALIZATION_VARIANTL_H

#include <volition/common.h>
#include <volition/FNV1a.h>

namespace Volition {

//================================================================//
// Variant
//================================================================//
class Variant {
public:

    enum Type {
        TYPE_BOOL,
        TYPE_NUMBER,
        TYPE_STRING,
        TYPE_UNDEFINED,
    };

    Type        mType;
    double      mNumeric;
    string      mString;
    
    //----------------------------------------------------------------//
    operator bool () const {
        
        switch ( this->mType ) {
            case TYPE_BOOL:
            case TYPE_NUMBER:
                return ( this->mNumeric != 0 );
            
            case TYPE_STRING:
                return ( this->mString.size () > 0 );
                
            default:
                break;
        }
        return false;
    }
    
    //----------------------------------------------------------------//
    Variant& operator = (const Variant& other ) {
        
        this->mType     = other.mType;
        this->mNumeric  = other.mNumeric;
        this->mString   = other.mString;
        
        return *this;
    }
    
    //----------------------------------------------------------------//
    static Variant add ( const Variant& lval, const Variant& rval ) {

        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return Variant ( lval.mNumeric + rval.mNumeric );
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant booleanAnd ( const Variant& lval, const Variant& rval ) {
    
        return Variant ( lval && rval );
    }
    
    //----------------------------------------------------------------//
    static Variant booleanNot ( const Variant& operand ) {
    
        return Variant ( !operand );
    }
    
    //----------------------------------------------------------------//
    static Variant booleanOr ( const Variant& lval, const Variant& rval ) {
    
        return Variant ( lval || rval );
    }
    
    //----------------------------------------------------------------//
    static Variant booleanXor ( const Variant& lval, const Variant& rval ) {
    
        bool lbool = ( bool )lval;
        bool rbool = ( bool )rval;
        
        return Variant ( lbool != rbool );
    }
    
    //----------------------------------------------------------------//
    static Variant div ( const Variant& lval, const Variant& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return Variant ( lval.mNumeric / rval.mNumeric );
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant equal ( const Variant& lval, const Variant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return Variant ( lval.mNumeric == rval.mNumeric );
                
                case TYPE_STRING:
                    return Variant ( lval.mString == rval.mString );
                    
                default:
                    break;
            }
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant greater ( const Variant& lval, const Variant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return Variant ( lval.mNumeric > rval.mNumeric );
                
                case TYPE_STRING:
                    return Variant ( lval.mString > rval.mString );
                
                default:
                    break;
            }
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant greaterOrEqual ( const Variant& lval, const Variant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return Variant ( lval.mNumeric >= rval.mNumeric );
                
                case TYPE_STRING:
                    return Variant ( lval.mString >= rval.mString );
                
                default:
                    break;
            }
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant less ( const Variant& lval, const Variant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return Variant ( lval.mNumeric < rval.mNumeric );
                
                case TYPE_STRING:
                    return Variant ( lval.mString < rval.mString );
                
                default:
                    break;
            }
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant lessOrEqual ( const Variant& lval, const Variant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return Variant ( lval.mNumeric <= rval.mNumeric );
                
                case TYPE_STRING:
                    return Variant ( lval.mString <= rval.mString );
                
                default:
                    break;
            }
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant mod ( const Variant& lval, const Variant& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return Variant ( fmod ( lval.mNumeric, rval.mNumeric ));
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant mul ( const Variant& lval, const Variant& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return Variant ( lval.mNumeric * rval.mNumeric );
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant notEqual ( const Variant& lval, const Variant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return Variant ( lval.mNumeric != rval.mNumeric );
                
                case TYPE_STRING:
                    return Variant ( lval.mString != rval.mString );
                
                default:
                    break;
            }
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    static Variant sub ( const Variant& lval, const Variant& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return Variant ( lval.mNumeric - rval.mNumeric );
        }
        return Variant ();
    }
    
    //----------------------------------------------------------------//
    Variant () :
        mType ( TYPE_UNDEFINED ),
        mNumeric ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    Variant ( const Variant& variant ) :
        mType ( variant.mType ),
        mNumeric ( variant.mNumeric ),
        mString ( variant.mString ) {
    }
    
    //----------------------------------------------------------------//
    Variant ( bool value ) :
        mType ( TYPE_BOOL ),
        mNumeric ( value ? 1 : 0 ) {
    }
    
    //----------------------------------------------------------------//
    Variant ( double value ) :
        mType ( TYPE_NUMBER ),
        mNumeric ( value ) {
    }
    
    //----------------------------------------------------------------//
    Variant ( string value ) :
        mType ( TYPE_STRING ),
        mString ( value ) {
    }
    
    //----------------------------------------------------------------//
    Variant ( Poco::Dynamic::Var var ) :
        mType ( TYPE_UNDEFINED ),
        mNumeric ( 0 ) {
    
        if ( var.isNumeric ()) {
            this->mType = TYPE_NUMBER;
            this->mNumeric = var.extract < double >();
        }
        else if ( var.isBoolean ()) {
            this->mType = TYPE_BOOL;
            this->mNumeric = var.extract < bool >() ? 1 : 0;
        }
        else if ( var.isString ()) {
            this->mType = TYPE_STRING;
            this->mString = var.extract < string >();
        }
    }
    
    //----------------------------------------------------------------//
    ~Variant () {
    }
};

} // namespace Volition
#endif
