// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SQUAPVAL_H
#define VOLITION_SQUAPVAL_H

#include <volition/common.h>
#include <volition/FNV1a.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// SquapVal
//================================================================//
class SquapVal :
    public AbstractSerializable {
public:

    enum Type {
        TYPE_BOOL,
        TYPE_NUMBER,
        TYPE_STRING,
        TYPE_UNDEFINED,
    };

    double      mNumber;
    string      mString;
    Type        mType;
    
    //----------------------------------------------------------------//
    operator bool () const {
        
        switch ( this->mType ) {
            case TYPE_BOOL:
            case TYPE_NUMBER:
                return ( this->mNumber != 0 );
            
            case TYPE_STRING:
                return ( this->mString.size () > 0 );
                
            default:
                break;
        }
        return false;
    }
    
    //----------------------------------------------------------------//
    static SquapVal add ( const SquapVal& lval, const SquapVal& rval ) {

        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return SquapVal ( lval.mNumber + rval.mNumber );
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal booleanAnd ( const SquapVal& lval, const SquapVal& rval ) {
    
        return SquapVal ( lval && rval );
    }
    
    //----------------------------------------------------------------//
    static SquapVal booleanNot ( const SquapVal& operand ) {
    
        return SquapVal ( !operand );
    }
    
    //----------------------------------------------------------------//
    static SquapVal booleanOr ( const SquapVal& lval, const SquapVal& rval ) {
    
        return SquapVal ( lval || rval );
    }
    
    //----------------------------------------------------------------//
    static SquapVal booleanXor ( const SquapVal& lval, const SquapVal& rval ) {
    
        bool lbool = ( bool )lval;
        bool rbool = ( bool )rval;
        
        return SquapVal (( lbool != rbool ) && ( lbool || rbool ));
    }
    
    //----------------------------------------------------------------//
    static SquapVal div ( const SquapVal& lval, const SquapVal& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return SquapVal ( lval.mNumber / rval.mNumber );
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal equal ( const SquapVal& lval, const SquapVal& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return SquapVal ( lval.mNumber == rval.mNumber );
                
                case TYPE_STRING:
                    return SquapVal ( lval.mString == rval.mString );
                    
                default:
                    break;
            }
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal greater ( const SquapVal& lval, const SquapVal& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return SquapVal ( lval.mNumber > rval.mNumber );
                
                case TYPE_STRING:
                    return SquapVal ( lval.mString > rval.mString );
                
                default:
                    break;
            }
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal greaterOrEqual ( const SquapVal& lval, const SquapVal& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return SquapVal ( lval.mNumber >= rval.mNumber );
                
                case TYPE_STRING:
                    return SquapVal ( lval.mString >= rval.mString );
                
                default:
                    break;
            }
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal less ( const SquapVal& lval, const SquapVal& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return SquapVal ( lval.mNumber < rval.mNumber );
                
                case TYPE_STRING:
                    return SquapVal ( lval.mString < rval.mString );
                
                default:
                    break;
            }
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal lessOrEqual ( const SquapVal& lval, const SquapVal& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return SquapVal ( lval.mNumber <= rval.mNumber );
                
                case TYPE_STRING:
                    return SquapVal ( lval.mString <= rval.mString );
                
                default:
                    break;
            }
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal mod ( const SquapVal& lval, const SquapVal& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return SquapVal ( fmod ( lval.mNumber, rval.mNumber ));
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal mul ( const SquapVal& lval, const SquapVal& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return SquapVal ( lval.mNumber * rval.mNumber );
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal notEqual ( const SquapVal& lval, const SquapVal& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return SquapVal ( lval.mNumber != rval.mNumber );
                
                case TYPE_STRING:
                    return SquapVal ( lval.mString != rval.mString );
                
                default:
                    break;
            }
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    static SquapVal sub ( const SquapVal& lval, const SquapVal& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return SquapVal ( lval.mNumber - rval.mNumber );
        }
        return SquapVal ();
    }
    
    //----------------------------------------------------------------//
    SquapVal () :
        mType ( TYPE_UNDEFINED ),
        mNumber ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    SquapVal ( bool value ) :
        mType ( TYPE_BOOL ),
        mNumber ( value ? 1 : 0 ) {
    }
    
    //----------------------------------------------------------------//
    SquapVal ( double value ) :
        mType ( TYPE_NUMBER ),
        mNumber ( value ) {
    }
    
    //----------------------------------------------------------------//
    SquapVal ( string value ) :
        mType ( TYPE_STRING ),
        mString ( value ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
    
        string type = serializer.serializeIn < string >( "type", "" );
        assert ( type.size ());
        
        switch ( FNV1a::hash_64 ( type.c_str ())) {
            case FNV1a::const_hash_64 ( "NUMBER" ):
                //*this = SquapVal ( serializer.serializeIn < double >( "value", 0 ));
                break;
            case FNV1a::const_hash_64 ( "STRING" ):
                *this = SquapVal ( serializer.serializeIn < string >( "value", "" ));
                break;
        }
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        assert ( false ); // unsupported
    }
};

} // namespace Volition
#endif
