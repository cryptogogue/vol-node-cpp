// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_SCHEMAVERSION_H
#define VOLITION_SCHEMAVERSION_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// SchemaVersion
//================================================================//
class SchemaVersion :
    public AbstractSerializable {
public:

    string      mRelease;
    u64         mMajor;
    u64         mMinor;
    u64         mRevision;
    
    //----------------------------------------------------------------//
    bool operator < ( const SchemaVersion& rhs ) const {
        return (
            ( this->mMajor <= rhs.mMajor ) &&
            ( this->mMinor <= rhs.mMinor ) &&
            ( this->mRevision < rhs.mRevision )
        );
    }
    
    //----------------------------------------------------------------//
    operator bool () const {
    
        return (( this->mMajor > 0 ) || ( this->mMinor > 0 ) || ( this->mMinor > 0 ) || ( this->mRelease.size () > 0 ));
    }
    
    //----------------------------------------------------------------//
    bool checkNext ( const SchemaVersion& next ) const {
    
        bool r0 = ( this->mRelease.size () > 0 );
        bool r1 = ( next.mRelease.size () > 0 );
    
        return (( r0 || r1 ) && (( !r0 ) || ( *this < next )));
    }
    
    //----------------------------------------------------------------//
    void compose ( const SchemaVersion& next ) {
    
        if ( next.mRelease.size () > 0 ) {
            this->mRelease = next.mRelease;
        }
        this->mMajor = next.mMajor;
        this->mMinor = next.mMinor;
        this->mRevision = next.mRevision;
    }
    
    //----------------------------------------------------------------//
    SchemaVersion () :
        mMajor ( 0 ),
        mMinor ( 0 ),
        mRevision ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {

        serializer.serialize ( "release",           this->mRelease );
        serializer.serialize ( "major",             this->mMajor );
        serializer.serialize ( "minor",             this->mMinor );
        serializer.serialize ( "revision",          this->mRevision );
    }

    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {

        serializer.serialize ( "release",           this->mRelease );
        serializer.serialize ( "major",             this->mMajor );
        serializer.serialize ( "minor",             this->mMinor );
        serializer.serialize ( "revision",          this->mRevision );
    }
};

} // namespace Volition
#endif
