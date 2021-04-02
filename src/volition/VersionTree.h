// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_VERSIONTREE_H
#define VOLITION_VERSIONTREE_H

#include <volition/common.h>
#include <volition/serialization/Serialization.h>

namespace Volition {

// NOTE: this is just a stub in case we want to implement a version (and/or merkel) tree for the inventory later.

//================================================================//
// VersionTreeNode
//================================================================//
class VersionTreeNode :
    public AbstractSerializable {
public:

    u32     mHeight;
    u32     mIndex;
    
    //----------------------------------------------------------------//
    static u32 findMSB ( u64 size ) {
        u32 msb = 0;
        while ( size >>= 1 ) {
            msb++;
        }
        return msb;
    }
    
    //----------------------------------------------------------------//
    VersionTreeNode getLeft ( u32 depth = 1 ) const {
    
        return VersionTreeNode ( this->mHeight - depth, this->mIndex >> depth );
    }
    
    //----------------------------------------------------------------//
    static u32 getHeight ( u64 size ) {
    
        u32 nextPow2 = VersionTreeNode::roundToNextPowerOfTwo ( size );
        return VersionTreeNode::findMSB ( size );
    }
    
    //----------------------------------------------------------------//
    VersionTreeNode getRight ( u32 depth = 1 ) const {
    
        return VersionTreeNode ( this->mHeight - depth, ( this->mIndex >> depth ) + depth );
    }
    
    //----------------------------------------------------------------//
    static u64 roundToNextPowerOfTwo ( u64 size ) {
    
        size--;
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        size |= size >> 32;
        size++;
        
        return size;
    }
    
    //----------------------------------------------------------------//
    VersionTreeNode () :
        mHeight ( 0 ),
        mIndex ( 0 ) {
    }
    
    //----------------------------------------------------------------//
    VersionTreeNode ( u32 height, u32 index ) :
        mHeight ( height ),
        mIndex ( index ) {
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) override {
        
        serializer.serialize ( "height",        this->mHeight );
        serializer.serialize ( "index",         this->mIndex );
    }
    
    //----------------------------------------------------------------//
    void AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const override {
        
        serializer.serialize ( "height",        this->mHeight );
        serializer.serialize ( "index",         this->mIndex );
    }
};

} // namespace Volition
#endif
