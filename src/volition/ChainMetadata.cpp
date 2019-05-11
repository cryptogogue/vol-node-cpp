// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/ChainMetadata.h>

namespace Volition {

//static const double THRESHOLD = 0.75;

//================================================================//
// ChainMetadata
//================================================================//

////----------------------------------------------------------------//
//void ChainMetadata::affirmCycle ( size_t cycleID ) {
//
//    this->mCycleMetadata.resize ( cycleID + 1 );
//}
//
////----------------------------------------------------------------//
//void ChainMetadata::affirmParticipant ( size_t cycleID, string participant ) {
//
//    this->affirmCycle ( cycleID );
//    CycleMetadata& metadata = this->mCycleMetadata [ cycleID ];
//    if ( metadata.mKnownParticipants.find ( participant ) == metadata.mKnownParticipants.end ()) {
//        metadata.mKnownParticipants.insert ( participant );
//    }
//}
//
////----------------------------------------------------------------//
//bool ChainMetadata::canEdit ( size_t cycleID, string minerID ) const {
//
//    if ( cycleID == 0 ) return false; // cannot change genesis cycle
//
//    size_t size = this->mCycleMetadata.size ();
//    
//    if ( size > 0 ) {
//        
//        if ( cycleID < ( size - 1 )) {
//        
//            size_t size0    = this->countParticipants ( cycleID, minerID );
//            size_t size1    = this->countParticipants ( cycleID + 1 );
//
//            float ratio     = ( float )size1 / ( float )size0;
//
//            return ratio <= THRESHOLD;
//        }
//    }
//    return true;
//}
//
////----------------------------------------------------------------//
//ChainMetadata::ChainMetadata () {
//}
//
////----------------------------------------------------------------//
//ChainMetadata::~ChainMetadata () {
//}
//
////----------------------------------------------------------------//
//size_t ChainMetadata::countParticipants ( size_t cycleID, string minerID ) const {
//
//    if ( cycleID < this->mCycleMetadata.size ()) {
//        const CycleMetadata& cycleMetadata = this->mCycleMetadata [ cycleID ];
//        return cycleMetadata.mKnownParticipants.size () + ( minerID.size () > 0 ? ( this->isParticipant ( cycleID, minerID ) ? 0 : 1 ) : 0 );
//    }
//    return 0;
//}
//
////----------------------------------------------------------------//
//bool ChainMetadata::isParticipant ( size_t cycleID, string minerID ) const {
//
//    if ( cycleID < this->mCycleMetadata.size ()) {
//        const CycleMetadata& metadata = this->mCycleMetadata [ cycleID ];
//        return ( metadata.mKnownParticipants.find ( minerID ) != metadata.mKnownParticipants.end ());
//    }
//    return false;
//}

//================================================================//
// overrides
//================================================================//

////----------------------------------------------------------------//
//void ChainMetadata::AbstractSerializable_serializeFrom ( const AbstractSerializerFrom& serializer ) {
//
//    serializer.serialize ( "metadata", this->mCycleMetadata );
//}
//
////----------------------------------------------------------------//
//void ChainMetadata::AbstractSerializable_serializeTo ( AbstractSerializerTo& serializer ) const {
//
//    serializer.serialize ( "metadata", this->mCycleMetadata );
//}

} // namespace Volition
