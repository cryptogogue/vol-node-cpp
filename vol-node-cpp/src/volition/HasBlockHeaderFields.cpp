// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Format.h>
#include <volition/Digest.h>
#include <volition/HasBlockHeaderFields.h>

namespace Volition {

//================================================================//
// HasBlockHeaderFields
//================================================================//

//----------------------------------------------------------------//
Digest HasBlockHeaderFields::calculateCharm ( const Digest& pose, const Digest& visage ) {

    // CHARM = POSE ^ VISAGE

//    printf ( "POSE: %s\n", pose.toHex ().substr ( 0, 6 ).c_str ());
//    printf ( "VISAGE: %s\n", visage.toHex ().substr ( 0, 6 ).c_str ());

    Digest charm;
    charm.resize ( CHARM_SIZE );
    
    size_t poseSize     = pose.size ();
    size_t visageSize   = visage.size ();
    
    for ( size_t i = 0; i < CHARM_SIZE; ++i ) {
    
        u8 a = poseSize ? pose [ i % poseSize ] : 0;
        u8 v = visageSize ? visage [ i % visageSize ] : 0;
    
        charm [ i ] = a ^ v;
    }
    
//    printf ( "CHARM:  %s\n", charm.toHex ().c_str ());
    
    return charm;
}

//----------------------------------------------------------------//
int HasBlockHeaderFields::compare ( const HasBlockHeaderFields& block0, const HasBlockHeaderFields& block1 ) {

    const BlockHeaderFields& fields0 = block0.getFields ();
    const BlockHeaderFields& fields1 = block1.getFields ();

    assert ( fields0.mHeight == fields1.mHeight );

    if ( block0.equals ( block1 )) return 0;

    return HasBlockHeaderFields::compare ( fields0.mCharm, fields1.mCharm );
}

//----------------------------------------------------------------//
int HasBlockHeaderFields::compare ( const Digest& charm0, const Digest& charm1 ) {

    string hex0 = charm0.toHex ();
    string hex1 = charm1.toHex ();

//    printf ( "charm0 (%s): %s\n", block0.getMinerID ().c_str (), hex0.c_str ());
//    printf ( "charm1 (%s): %s\n", block1.getMinerID ().c_str (), hex1.c_str ());
        
    int result = hex0.compare ( hex1 );
    return result < 0 ? -1 : result > 0 ? 1 : 0;
}

//----------------------------------------------------------------//
bool HasBlockHeaderFields::equals ( const HasBlockHeaderFields& rhs ) const {

    const BlockHeaderFields& fields = this->getFields ();
    const BlockHeaderFields& rhsFields = rhs.getFields ();

    return (( fields.mHeight == rhsFields.mHeight ) && ( fields.mDigest == rhsFields.mDigest ));
}

//----------------------------------------------------------------//
string HasBlockHeaderFields::formatPoseString ( string prevPose ) const {

    const BlockHeaderFields& fields = this->getFields ();
    
    return Format::write ( "%s:%zu:%s", fields.mMinerID.c_str (), fields.mHeight, prevPose.c_str ());
}

//----------------------------------------------------------------//
string HasBlockHeaderFields::getCharmTag () const {

    return this->getCharm ().toHex ().substr ( 0, 6 );
}

//----------------------------------------------------------------//
const BlockHeaderFields& HasBlockHeaderFields::getFields () const {
    return HasBlockHeader_getFields ();
}

//----------------------------------------------------------------//
Digest HasBlockHeaderFields::getNextCharm ( const Digest& visage ) const {

    return HasBlockHeaderFields::calculateCharm ( this->getPose (), visage );
}

//----------------------------------------------------------------//
time_t HasBlockHeaderFields::getNextTime () const {

    const BlockHeaderFields& fields = this->getFields ();
    
    return fields.mTime + fields.mBlockDelay;
}

//----------------------------------------------------------------//
HasBlockHeaderFields::HasBlockHeaderFields () {
}

//----------------------------------------------------------------//
HasBlockHeaderFields::~HasBlockHeaderFields () {
}

//----------------------------------------------------------------//
Digest HasBlockHeaderFields::hashPose ( string prevPose ) const {

    return Digest ( this->formatPoseString ( prevPose ));
}

//----------------------------------------------------------------//
bool HasBlockHeaderFields::isGenesis () const {

    return ( this->getHeight () == 0 );
}

//----------------------------------------------------------------//
bool HasBlockHeaderFields::isInRewriteWindow ( time_t now ) const {

    const BlockHeaderFields& fields = this->getFields ();
    
    double diff = difftime ( now, fields.mTime );
    return diff < fields.mRewriteWindow;
}

//----------------------------------------------------------------//
bool HasBlockHeaderFields::isParent ( const HasBlockHeaderFields& block ) const {

    return ( this->getDigest () == block.getPrevDigest ()); // TODO: does not need to be constant time
}

} // namespace Volition
