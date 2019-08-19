// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETFIELDVARIANT_H
#define VOLITION_ASSETFIELDVARIANT_H

#include <volition/common.h>
#include <volition/FNV1a.h>

namespace Volition {

//================================================================//
// AssetFieldVariant
//================================================================//
class AssetFieldVariant {
public:
    
    enum Type : u64 {
        TYPE_BOOL       = FNV1a::const_hash_64 ( "BOOLEAN" ),
        TYPE_NUMBER     = FNV1a::const_hash_64 ( "NUMERIC" ),
        TYPE_STRING     = FNV1a::const_hash_64 ( "STRING" ),
        TYPE_UNDEFINED  = FNV1a::const_hash_64 ( "UNDEFINED" ),
    };

private:

    Type        mType;
    double      mNumeric;
    string      mString;

public:

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
    AssetFieldVariant& operator = (const AssetFieldVariant& other ) {
        
        this->mType     = other.mType;
        this->mNumeric  = other.mNumeric;
        this->mString   = other.mString;
        
        return *this;
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant add ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {

        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldVariant ( lval.mNumeric + rval.mNumeric );
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    AssetFieldVariant () :
        mType ( TYPE_UNDEFINED ),
        mNumeric ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldVariant ( const AssetFieldVariant& variant ) :
        mType ( variant.mType ),
        mNumeric ( variant.mNumeric ),
        mString ( variant.mString ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldVariant ( bool value ) :
        mType ( TYPE_BOOL ),
        mNumeric ( value ? 1 : 0 ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldVariant ( double value ) :
        mType ( TYPE_NUMBER ),
        mNumeric ( value ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldVariant ( string value ) :
        mType ( TYPE_STRING ),
        mString ( value ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldVariant ( Poco::Dynamic::Var var ) :
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
    ~AssetFieldVariant () {
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant booleanAnd ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        return AssetFieldVariant ( lval && rval );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant booleanNot ( const AssetFieldVariant& operand ) {
    
        return AssetFieldVariant ( !operand );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant booleanOr ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        return AssetFieldVariant ( lval || rval );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant booleanXor ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        bool lbool = ( bool )lval;
        bool rbool = ( bool )rval;
        
        return AssetFieldVariant ( lbool != rbool );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant div ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldVariant ( lval.mNumeric / rval.mNumeric );
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant equal ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldVariant ( lval.mNumeric == rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldVariant ( lval.mString == rval.mString );
                    
                default:
                    break;
            }
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant greater ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldVariant ( lval.mNumeric > rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldVariant ( lval.mString > rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant greaterOrEqual ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldVariant ( lval.mNumeric >= rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldVariant ( lval.mString >= rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    Type getType () const {
    
        return this->mType;
    }
    
    //----------------------------------------------------------------//
    static string getTypeName ( Type type ) {
    
        switch ( type ) {
            case TYPE_BOOL:         return "BOOLEAN";
            case TYPE_NUMBER:       return "NUMERIC";
            case TYPE_STRING:       return "STRING";
            case TYPE_UNDEFINED:    return "UNDEFINED";
        }
        return "";
    }
    
    //----------------------------------------------------------------//
    bool isBool () const {
        return ( this->mType == Type::TYPE_BOOL );
    }
    
    //----------------------------------------------------------------//
    bool isNumber () const {
        return ( this->mType == Type::TYPE_NUMBER );
    }
    
    //----------------------------------------------------------------//
    bool isString () const {
        return ( this->mType == Type::TYPE_STRING );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant less ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldVariant ( lval.mNumeric < rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldVariant ( lval.mString < rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant lessOrEqual ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldVariant ( lval.mNumeric <= rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldVariant ( lval.mString <= rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant mod ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldVariant ( fmod ( lval.mNumeric, rval.mNumeric ));
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant mul ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldVariant ( lval.mNumeric * rval.mNumeric );
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant notEqual ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldVariant ( lval.mNumeric != rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldVariant ( lval.mString != rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldVariant ();
    }
    
    //----------------------------------------------------------------//
    void serializeValue ( const AbstractSerializerFrom& serializer, Type type, string key ) {
    
        this->mType = type;
    
        switch ( this->mType ) {
            case Type::TYPE_BOOL:
                bool value;
                serializer.serialize ( key, value );
                this->mNumeric = value ? 1 : 0;
                break;
            case Type::TYPE_NUMBER:
                serializer.serialize ( key, this->mNumeric );
                break;
            case Type::TYPE_STRING:
                serializer.serialize ( key, this->mString );
                break;
            default:
                break;
        }
    }
    
    //----------------------------------------------------------------//
    void serializeValue ( AbstractSerializerTo& serializer, string key ) const {
    
        switch ( this->mType ) {
            case Type::TYPE_BOOL:
                serializer.serialize ( key, ( bool )( this->mNumeric != 0 ? true : false ));
                break;
            case Type::TYPE_NUMBER:
                serializer.serialize ( key, this->mNumeric );
                break;
            case Type::TYPE_STRING:
                serializer.serialize ( key, this->mString );
                break;
            default:
                break;
        }
    }

    //----------------------------------------------------------------//
    bool strictBoolean () const {
        assert ( this->mType == Type::TYPE_BOOL );
        return ( this->mNumeric != 0 );
    }
    
    //----------------------------------------------------------------//
    bool strictNumber () const {
        assert ( this->mType == Type::TYPE_NUMBER );
        return this->mNumeric;
    }
    
    //----------------------------------------------------------------//
    string strictString () const {
        assert ( this->mType == Type::TYPE_STRING );
        return this->mString;
    }
    
    //----------------------------------------------------------------//
    static AssetFieldVariant sub ( const AssetFieldVariant& lval, const AssetFieldVariant& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldVariant ( lval.mNumeric - rval.mNumeric );
        }
        return AssetFieldVariant ();
    }
};

} // namespace Volition
#endif
