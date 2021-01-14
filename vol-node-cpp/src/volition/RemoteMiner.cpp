// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/Block.h>
#include <volition/Digest.h>
#include <volition/HTTPMiningMessenger.h>
#include <volition/RemoteMiner.h>
#include <volition/Transaction.h>
#include <volition/Transactions.h>
#include <volition/UnsecureRandom.h>

namespace Volition {

//================================================================//
// RemoteMiner
//================================================================//

//----------------------------------------------------------------//
RemoteMiner::RemoteMiner () :
    mNetworkState ( STATE_NEW ),
    mHeight ( 0 ),
    mForward ( true ) {
}

//----------------------------------------------------------------//
RemoteMiner::~RemoteMiner () {
}

//----------------------------------------------------------------//
void RemoteMiner::reset () {

    this->mHeaderQueue.clear ();

    this->mTag.reset ();
    this->mImproved.reset ();

    this->mHeight           = 0;
    this->mForward          = true;
}

//----------------------------------------------------------------//
void RemoteMiner::setError ( string message ) {

    this->reset ();

    this->mNetworkState     = STATE_ERROR;
    this->mMessage          = message;
}

//----------------------------------------------------------------//
void RemoteMiner::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
    this->mTag.setName ( Format::write ( "%s'", minerID.c_str ()));
    this->mImproved.setName ( Format::write ( "~%s'", minerID.c_str ()));
}

//----------------------------------------------------------------//
void RemoteMiner::updateHeaders ( AbstractBlockTree& blockTree ) {
    
    // process the queue
    size_t accepted = 0;
    if ( this->mHeaderQueue.size ()) {
        
        // visit each header in the cache and try to apply it.
        while ( this->mHeaderQueue.size ()) {

            shared_ptr < const BlockHeader > header = this->mHeaderQueue.begin ()->second;
            
            kBlockTreeAppendResult canAppend = blockTree.checkAppend ( *header );
            
            switch ( canAppend ) {
            
                case kBlockTreeAppendResult::APPEND_OK:
                case kBlockTreeAppendResult::ALREADY_EXISTS:
                    blockTree.affirmHeader ( this->mTag, header );
                    this->mHeaderQueue.erase ( this->mHeaderQueue.begin ());
                    accepted++;
                    break;
                
                case kBlockTreeAppendResult::MISSING_PARENT:
                    if ( accepted == 0 ) {
                        // if there's anything left in the queue, back up and get an earlier batch of blocks.
                        this->mHeight = this->mHeaderQueue.begin ()->second->getHeight ();
                        this->mForward = false;
                    }
                    this->reset ();
                    return;
                
                case kBlockTreeAppendResult::REFUSED:
                case kBlockTreeAppendResult::TOO_SOON:
                    this->reset ();
                    break;
            }
        }
    }
    
    BlockTreeCursor bestCursor = this->mTag.getCursor ();
    if ( bestCursor.hasHeader ()) {
        // nothing in the queue, so get the next batch of blocks.
        this->mHeight = bestCursor.getHeight () + 1; // this doesn't really matter.
        this->mForward = true;
    }
}

} // namespace Volition
