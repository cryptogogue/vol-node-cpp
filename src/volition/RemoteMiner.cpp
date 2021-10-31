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
void RemoteMiner::processHeaders ( const MiningMessengerResponse& response, time_t now, u64 acceptedRelease ) {
    
    const list < shared_ptr < const BlockHeader >>& headerList = response.mHeaders;
    
    AbstractBlockTree& blockTree = *this->mMiner.mBlockTree;
    
    list < shared_ptr < const BlockHeader >>::const_iterator headerIt = headerList.begin ();
    for ( ; headerIt != headerList.end (); ++headerIt ) {
                
        shared_ptr < const BlockHeader > header = *headerIt;
        
        // ignore missing headers and headers from the future.
        if ( !header || ( now < header->getTime ())) return;
        
        // if genesis hashes don't match, we have a real problem.
        if (( header->getHeight () == 0 ) && ( header->getDigest ().toHex () != this->mMiner.mLedger->getGenesisHash ())) {
            this->setError ();
            return;
        }
        
        // try to append the new header.
        switch ( blockTree.checkAppend ( *header, acceptedRelease )) {
            
            case kBlockTreeAppendResult::APPEND_OK:
            case kBlockTreeAppendResult::ALREADY_EXISTS:
            
                blockTree.affirmHeader ( this->mTag, header );
                this->mHeight = header->getHeight ();
                this->mRewind = 0;
                break;
            
            case kBlockTreeAppendResult::MISSING_PARENT:
                
                this->mRewind = this->mRewind ? this->mRewind : 1;
                return;
            
            case kBlockTreeAppendResult::INCOMPATIBLE:
            case kBlockTreeAppendResult::TOO_SOON:
                
                this->mRewind = 0;
                return;
        }
    }
}

//----------------------------------------------------------------//
bool RemoteMiner::receiveResponse ( const MiningMessengerResponse& response, time_t now, u64 acceptedRelease ) {

    const MiningMessengerRequest& request   = response.mRequest;
    string url                              = response.mRequest.mMinerURL;
    MiningMessengerResponse::Status status  = response.mStatus;
    
    // TODO: these could be set deliberately as an attack
    assert ( url == this->mURL );
    
    if ( status != MiningMessengerResponse::STATUS_OK ) {
        this->mState = STATE_OFFLINE;
        return true;
    }
    
    switch ( request.mRequestType ) {
        
        case MiningMessengerRequest::REQUEST_HEADERS: {
                                        
            if ( this->mState == STATE_WAITING_FOR_HEADERS ) {
                
                this->mAcceptedRelease      = response.mAcceptedRelease;
                this->mNextRelease  = response.mNextRelease;
                this->processHeaders ( response, now, acceptedRelease );
                this->mState = STATE_ONLINE;
            }
            break;
        }
        
        case MiningMessengerRequest::REQUEST_MINER_INFO: {

            if ( this->mState == STATE_WAITING_FOR_INFO ) {
                
                // if genesis hashes don't match, we have a real problem.
                if ( response.mGenesisHash != this->mMiner.mLedger->getGenesisHash ()) {
                    this->setError ();
                }
                else {
                
                    this->setMinerID ( response.mMinerID );
                    this->mAcceptedRelease      = response.mAcceptedRelease;
                    this->mNextRelease  = response.mNextRelease;
                    this->mMiner.mRemoteMinersByID [ this->getMinerID ()] = this->shared_from_this ();
                    
                    BlockTreeCursor cursor = *this->mTag;
                    if ( cursor.hasHeader ()) {
                        this->mHeight = cursor.getHeight ();
                    }
                    this->mState = STATE_ONLINE;
                }
            }
            break;
        }
        
        default:
            break;
    }
    
    return true;
}

//----------------------------------------------------------------//
RemoteMiner::RemoteMiner ( Miner& miner ) :
    mMiner ( miner ),
    mState ( STATE_OFFLINE ),
    mAcceptedRelease ( 0 ),
    mNextRelease ( 0 ),
    mRewind ( 0 ),
    mHeight ( 0 ) {
}

//----------------------------------------------------------------//
RemoteMiner::~RemoteMiner () {
}

//----------------------------------------------------------------//
void RemoteMiner::report ( u64 minHeight, u64 maxHeight ) const {

    string minerIDString;

    if ( this->mMinerID.size ()) {
        minerIDString = ( this->mAcceptedRelease == this->mNextRelease ) ?
            Format::write ( "%s.%d", this->mMinerID.c_str (), ( int )this->mAcceptedRelease ) :
            Format::write ( "%s.%d->%d", this->mMinerID.c_str (), ( int )this->mAcceptedRelease, ( int )this->mNextRelease );
    }
    else {
        minerIDString = "[??].?";
    }

    cc8* minerIDCString = minerIDString.c_str ();
    cc8* url = this->mURL.c_str ();

    switch ( this->mState ) {
    
        case STATE_OFFLINE:
            LGN_LOG ( VOL_FILTER_MINING_REPORT, INFO, "%s: OFFLINE (%s)", minerIDCString, url );
            break;
        
        case STATE_WAITING_FOR_INFO:
            LGN_LOG ( VOL_FILTER_MINING_REPORT, INFO, "%s: WAITING (%s)", minerIDCString, url );
            break;
        
        case STATE_ONLINE:
        case STATE_WAITING_FOR_HEADERS:
            
            if ( this->mRewind ) {
                LGN_LOG ( VOL_FILTER_MINING_REPORT, INFO, "%s: REWINDING (height: %d)", minerIDCString, ( int )this->mHeight );
            }
            else {
            
                 BlockTreeCursor remoteCursor = *this->mTag;
                 if ( remoteCursor.hasHeader ()) {
                    LGN_LOG ( VOL_FILTER_MINING_REPORT, INFO,
                        "%s - %d: %s",
                        minerIDCString,
                        ( int )remoteCursor.getHeight (),
                        remoteCursor.writeBranch ( minHeight, maxHeight ).c_str ()
                    );
                }
                else {
                    LGN_LOG ( VOL_FILTER_MINING_REPORT, INFO, "%s: MISSING CURSOR", minerIDCString );
                }
            }
            break;
        
        case STATE_ERROR:
            // TODO: genesis block mismatch is the only error we check for right now. Handler/report more.
            LGN_LOG ( VOL_FILTER_MINING_REPORT, INFO, "%s: ERROR - Genesis block mismatch.", minerIDCString );
            break;
    }
}

//----------------------------------------------------------------//
void RemoteMiner::reset () {
    this->mTag.reset ();
}

//----------------------------------------------------------------//
void RemoteMiner::setError () {

    this->reset ();
    this->mState    = STATE_ERROR;
}

//----------------------------------------------------------------//
void RemoteMiner::setMinerID ( string minerID ) {

    if ( this->mMinerID == minerID ) return;

    this->mMinerID = minerID;
    this->mTag.setName ( Format::write ( "%s'", minerID.c_str ()));
    this->mTag.setTree ( this->mMiner.mBlockTree.get ());
}

//----------------------------------------------------------------//
void RemoteMiner::update ( u64 acceptedRelease ) {
    
    LGN_LOG ( VOL_FILTER_CONSENSUS, INFO, __PRETTY_FUNCTION__ );
    
    AbstractMiningMessenger& messenger = *this->mMiner.getMessenger ();
    
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
            
            messenger.enqueueHeadersRequest ( this->mURL, this->mHeight, acceptedRelease );
            this->mState = STATE_WAITING_FOR_HEADERS;
            break;
        
        default:
            break;
    }
}

} // namespace Volition
