// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ASSETFIELDVALUE
#define VOLITION_ASSETFIELDVALUE

#include <volition/common.h>
#include <volition/FNV1a.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

//================================================================//
// AssetFieldValue
//================================================================//
class AssetFieldValue :
    public virtual AbstractSerializable {
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
    AssetFieldValue& operator = (const AssetFieldValue& other ) {
        
        this->mType     = other.mType;
        this->mNumeric  = other.mNumeric;
        this->mString   = other.mString;
        
        return *this;
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue add ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {

        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldValue ( lval.mNumeric + rval.mNumeric );
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue () :
        mType ( TYPE_UNDEFINED ),
        mNumeric ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( const AssetFieldValue& variant ) :
        mType ( variant.mType ),
        mNumeric ( variant.mNumeric ),
        mString ( variant.mString ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( bool value ) :
        mType ( TYPE_BOOL ),
        mNumeric ( value ? 1 : 0 ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( double value ) :
        mType ( TYPE_NUMBER ),
        mNumeric ( value ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( string value ) :
        mType ( TYPE_STRING ),
        mString ( value ) {
    }
    
    //----------------------------------------------------------------//
    AssetFieldValue ( Poco::Dynamic::Var var ) :
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
    ~AssetFieldValue () {
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue booleanAnd ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        return AssetFieldValue ( lval && rval );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue booleanNot ( const AssetFieldValue& operand ) {
    
        return AssetFieldValue ( !operand );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue booleanOr ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        return AssetFieldValue ( lval || rval );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue booleanXor ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        bool lbool = ( bool )lval;
        bool rbool = ( bool )rval;
        
        return AssetFieldValue ( lbool != rbool );
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue div ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldValue ( lval.mNumeric / rval.mNumeric );
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue equal ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldValue ( lval.mNumeric == rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldValue ( lval.mString == rval.mString );
                    
                default:
                    break;
            }
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue greater ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldValue ( lval.mNumeric > rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldValue ( lval.mString > rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue greaterOrEqual ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldValue ( lval.mNumeric >= rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldValue ( lval.mString >= rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldValue ();
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
    static AssetFieldValue less ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldValue ( lval.mNumeric < rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldValue ( lval.mString < rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue lessOrEqual ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldValue ( lval.mNumeric <= rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldValue ( lval.mString <= rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue mod ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldValue ( fmod ( lval.mNumeric, rval.mNumeric ));
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue mul ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldValue ( lval.mNumeric * rval.mNumeric );
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue notEqual ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if ( lval.mType == rval.mType ) {
            switch ( lval.mType ) {
                case TYPE_BOOL:
                case TYPE_NUMBER:
                    return AssetFieldValue ( lval.mNumeric != rval.mNumeric );
                
                case TYPE_STRING:
                    return AssetFieldValue ( lval.mString != rval.mString );
                
                default:
                    break;
            }
        }
        return AssetFieldValue ();
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
    double strictNumber () const {
        assert ( this->mType == Type::TYPE_NUMBER );
        return this->mNumeric;
    }
    
    //----------------------------------------------------------------//
    string strictString () const {
        assert ( this->mType == Type::TYPE_STRING );
        return this->mString;
    }
    
    //----------------------------------------------------------------//
    static AssetFieldValue sub ( const AssetFieldValue& lval, const AssetFieldValue& rval ) {
    
        if (( lval.mType == TYPE_NUMBER ) && ( rval.mType == TYPE_NUMBER )) {
            return AssetFieldValue ( lval.mNumeric - rval.mNumeric );
        }
        return AssetFieldValue ();
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        string typeStr;
        serializer.serialize ( "type", typeStr );
        Type type = ( Type )FNV1a::hash_64 ( typeStr.c_str ());
        
        this->serializeValue ( serializer, type, "value" );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        string typeStr = AssetFieldValue::getTypeName ( this->getType ());
        serializer.serialize ( "type", typeStr );
        
        this->serializeValue ( serializer, "value" );
    }
};

} // namespace Volition
#endif
