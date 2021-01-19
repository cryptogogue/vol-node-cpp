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
bool RemoteMiner::canFetchInfo () const {

    return (( this->mIsBusy == false ) && (( this->mNetworkState != STATE_ONLINE )));
}

//----------------------------------------------------------------//
bool RemoteMiner::canFetchHeaders () const {

    return (( this->mIsBusy == false ) && ( this->mNetworkState == STATE_ONLINE ));
}

//----------------------------------------------------------------//
void RemoteMiner::receiveResponse ( Miner& miner, const MiningMessengerResponse& response, time_t now ) {

    const MiningMessengerRequest& request   = response.mRequest;
    string url                              = response.mRequest.mMinerURL;
    MiningMessengerResponse::Status status  = response.mStatus;
    
    // TODO: these could be set deliberately as an attack
    assert ( url == this->mURL );
    
    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_HEADERS:
        case MiningMessengerRequest::REQUEST_PREV_HEADERS: {
                                        
            list < shared_ptr < const BlockHeader >>::const_iterator headerIt = response.mHeaders.cbegin ();
            for ( ; headerIt != response.mHeaders.cend (); ++headerIt ) {
                
                shared_ptr < const BlockHeader > header = *headerIt;
                if ( !header ) continue;
                if ( header->getTime () > now ) continue; // ignore headers from the future
                
                if ( header->getHeight () == 0 ) {
                    if ( header->getDigest ().toHex () != miner.mLedger->getGenesisHash ()) {
                        this->setError ( "Unrecoverable error: genesis block mismatch." );
                        break;
                    }
                }
                this->mHeaderQueue [ header->getHeight ()] = header;
            }
            
            this->updateHeaders ( *miner.mBlockTree );
            
            this->mIsBusy = false;
            break;
        }
        
        case MiningMessengerRequest::REQUEST_MINER_INFO: {

            if ( status == MiningMessengerResponse::STATUS_OK ) {
                this->setMinerID ( response.mMinerID );
                miner.mRemoteMinersByID [ this->getMinerID ()] = this->shared_from_this ();
            }
            this->mIsBusy = false;
            break;
        }
        
        default: break;
    }
    
    if ( status == MiningMessengerResponse::STATUS_OK ) {
        this->mNetworkState = RemoteMiner::STATE_ONLINE;
    }
    else {
        this->setError ();
    }
}

//----------------------------------------------------------------//
RemoteMiner::RemoteMiner () :
    mNetworkState ( STATE_NEW ),
    mHeight ( 0 ),
    mIsBusy ( false ),
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
                        this->mTag.reset ();
                        this->mImproved.reset ();
                    }
                    else {
                        this->reset ();
                    }
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

//----------------------------------------------------------------//
void RemoteMiner::update ( AbstractMiningMessenger& messenger ) {

    if ( this->canFetchHeaders ()) {
        this->mIsBusy = true;
        messenger.enqueueHeaderRequest ( this->mURL, this->mHeight, this->mForward );
    }
}

} // namespace Volition
