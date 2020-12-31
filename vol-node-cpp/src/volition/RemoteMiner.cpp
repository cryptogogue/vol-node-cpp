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

    this->mHeight           = 0;
    this->mTag              = NULL;
    this->mImproved         = NULL;
    this->mForward          = true;
    
    this->mHeaderQueue.clear ();
}

//----------------------------------------------------------------//
void RemoteMiner::setError ( string message ) {

    this->mNetworkState     = STATE_ERROR;
    this->mMessage          = message;
    this->mHeight           = 0;
    this->mTag              = NULL;
    this->mImproved         = NULL;
    this->mForward          = true;
    
    this->mHeaderQueue.clear ();
}

//----------------------------------------------------------------//
void RemoteMiner::setMinerID ( string minerID ) {

    this->mMinerID = minerID;
    this->mTag.setTagName ( Format::write ( "~%s'", minerID.c_str ()));
    this->mImproved.setTagName ( Format::write ( "~%s'", minerID.c_str ()));
}

//----------------------------------------------------------------//
void RemoteMiner::updateHeaders ( BlockTree& blockTree ) {
    
    // process the queue
    size_t accepted = 0;
    if ( this->mHeaderQueue.size ()) {
        
        // if 'tag' gets overwritten, 'thumb' will hang on to any nodes we might need later.
//        BlockTreeNode::ConstPtr thumb = this->mTag;
        
        // visit each header in the cache and try to apply it.
        while ( this->mHeaderQueue.size ()) {

            shared_ptr < const BlockHeader > header = this->mHeaderQueue.begin ()->second;
            
            BlockTree::CanAppend canAppend = blockTree.checkAppend ( *header );
            
            switch ( canAppend ) {
            
                case BlockTree::APPEND_OK:
                case BlockTree::ALREADY_EXISTS:
                    this->mTag = blockTree.affirmHeader ( this->mTag, header );
                    this->mHeaderQueue.erase ( this->mHeaderQueue.begin ());
                    accepted++;
                    break;
                
                case BlockTree::MISSING_PARENT:
                    if ( accepted == 0 ) {
                        // if there's anything left in the queue, back up and get an earlier batch of blocks.
                        this->mHeight = this->mHeaderQueue.begin ()->second->getHeight ();
                        this->mForward = false;
                    }
                    this->reset ();
                    return;
                
                case BlockTree::REFUSED:
                case BlockTree::TOO_SOON:
                    this->reset ();
                    break;
            }
        }
    }
    
    if ( this->mTag ) {
        // nothing in the queue, so get the next batch of blocks.
        this->mHeight = ( **this->mTag ).getHeight () + 1; // this doesn't really matter.
        this->mForward = true;
    }
}

} // namespace Volition
