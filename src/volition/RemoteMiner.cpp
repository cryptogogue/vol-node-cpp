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
bool RemoteMiner::isContributor () const {

    return ( this->isOnline () && this->mTag.hasTree ());
}

//----------------------------------------------------------------//
bool RemoteMiner::isOnline () const {

    return ( this->mState == STATE_ONLINE || this->mState == STATE_WAITING_FOR_HEADERS );
}

//----------------------------------------------------------------//
void RemoteMiner::processHeaders ( Miner& miner, const MiningMessengerResponse& response, time_t now ) {
    
    const list < shared_ptr < const BlockHeader >>& headerList = response.mHeaders;
    
    AbstractBlockTree& blockTree = *miner.mBlockTree;
    
    list < shared_ptr < const BlockHeader >>::const_iterator headerIt = headerList.begin ();
    for ( ; headerIt != headerList.end (); ++headerIt ) {
                
        shared_ptr < const BlockHeader > header = *headerIt;
        
        // ignore missing headers and headers from the future.
        if ( !header || ( now < header->getTime ())) return;
        
        // if genesis hashes don't match, we have a real problem.
        if (( header->getHeight () == 0 ) && ( header->getDigest ().toHex () != miner.mLedger->getGenesisHash ())) {
            this->setError ( "Unrecoverable error: genesis block mismatch." );
            return;
        }
        
        // try to append the new header.
        switch ( blockTree.checkAppend ( *header )) {
            
            case kBlockTreeAppendResult::APPEND_OK:
            case kBlockTreeAppendResult::ALREADY_EXISTS:
            
                blockTree.affirmHeader ( this->mTag, header );
                this->mHeight = header->getHeight ();
                this->mRewind = 0;
                break;
            
            case kBlockTreeAppendResult::MISSING_PARENT:
                
                this->mRewind = this->mRewind ? this->mRewind : 1;
                return;
            
            case kBlockTreeAppendResult::TOO_SOON:
                
                this->mRewind = 0;
                return;
        }
    }
}

//----------------------------------------------------------------//
void RemoteMiner::receiveResponse ( Miner& miner, const MiningMessengerResponse& response, time_t now ) {

    const MiningMessengerRequest& request   = response.mRequest;
    string url                              = response.mRequest.mMinerURL;
    MiningMessengerResponse::Status status  = response.mStatus;
    
    // TODO: these could be set deliberately as an attack
    assert ( url == this->mURL );
    
    if ( status != MiningMessengerResponse::STATUS_OK ) {
        this->mState = STATE_OFFLINE;
        return;
    }
    
    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_HEADERS: {
                                        
            if ( this->mState == STATE_WAITING_FOR_HEADERS ) {
                
                this->processHeaders ( miner, response, now );
                this->mState = STATE_ONLINE;
            }
            break;
        }
        
        case MiningMessengerRequest::REQUEST_MINER_INFO: {

            if ( this->mState == STATE_WAITING_FOR_INFO ) {
                
                this->setMinerID ( response.mMinerID );
                miner.mRemoteMinersByID [ this->getMinerID ()] = this->shared_from_this ();
                
                BlockTreeCursor cursor = miner.mBlockTree->restoreTag ( this->mTag );
                if ( cursor.hasHeader ()) {
                    this->mHeight = cursor.getHeight ();
                }
                this->mState = STATE_ONLINE;
            }
            break;
        }
        
        default:
            break;
    }
}

//----------------------------------------------------------------//
RemoteMiner::RemoteMiner () :
    mState ( STATE_OFFLINE ),
    mRewind ( 0 ),
    mHeight ( 0 ) {
}

//----------------------------------------------------------------//
RemoteMiner::~RemoteMiner () {
}

//----------------------------------------------------------------//
void RemoteMiner::report () const {

    cc8* minerID = this->mMinerID.size () ? this->mMinerID.c_str () : "[??]";
    cc8* url = this->mURL.c_str ();

    switch ( this->mState ) {
    
        case STATE_OFFLINE:
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: OFFLINE (%s)", minerID, url );
            break;
        
        case STATE_WAITING_FOR_INFO:
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: WAITING (%s)", minerID, url );
            break;
        
        case STATE_ONLINE:
        case STATE_WAITING_FOR_HEADERS:
            
            if ( this->mRewind ) {
                LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: REWINDING (height: %d)", minerID, ( int )this->mHeight );
            }
            else {
            
                 BlockTreeCursor remoteCursor = *this->mTag;
                 if ( remoteCursor.hasHeader ()) {
                    LGN_LOG ( VOL_FILTER_ROOT, INFO,
                        "%s - %d: %s",
                        minerID,
                        ( int )remoteCursor.getHeight (),
                        remoteCursor.writeBranch ().c_str ()
                    );
                }
                else {
                    LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: MISSING CURSOR", minerID );
                }
            }
            break;
        
        case STATE_ERROR:
            LGN_LOG ( VOL_FILTER_ROOT, INFO, "%s: UNRECOVERABLE", minerID );
            break;
    }
}

//----------------------------------------------------------------//
void RemoteMiner::reset () {
    this->mTag.reset ();
    this->mImproved.reset ();
}

//----------------------------------------------------------------//
void RemoteMiner::setError ( string message ) {

    this->reset ();

    this->mState    = STATE_ERROR;
    this->mMessage  = message;
}

//----------------------------------------------------------------//
void RemoteMiner::setMinerID ( string minerID ) {

    if ( this->mMinerID == minerID ) return;

    this->mMinerID = minerID;
    this->mTag.setName ( Format::write ( "%s'", minerID.c_str ()));
    this->mImproved.setName ( Format::write ( "~%s'", minerID.c_str ()));
}

//----------------------------------------------------------------//
void RemoteMiner::update ( AbstractMiningMessenger& messenger ) {
    
    switch ( this->mState ) {
    
        case STATE_OFFLINE:
            messenger.enqueueMinerInfoRequest ( this->mURL );
            this->mState = STATE_WAITING_FOR_INFO;
            break;
        
        case STATE_ONLINE:
        
            if ( this->mRewind ) {
                if ( this->mRewind < this->mHeight ) {
                    this->mHeight -= this->mRewind;
                    this->mRewind = this->mRewind * 2;
                }
                else {
                    this->mHeight = 0;
                }
            }
            
            messenger.enqueueHeadersRequest ( this->mURL, this->mHeight );
            this->mState = STATE_WAITING_FOR_HEADERS;
            break;
        
        default:
            break;
    }
}

} // namespace Volition
