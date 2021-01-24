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
void RemoteMiner::processHeaders ( Miner& miner, time_t now ) {
    
    AbstractBlockTree& blockTree = *miner.mBlockTree;
    
    size_t accepted = 0;
    while ( this->mHeaderList.size ()) {
        
        list < shared_ptr < const BlockHeader >>::const_iterator headerIt = this->mHeaderList.cbegin ();
        
        shared_ptr < const BlockHeader > header = *headerIt;
        
        if ( !header || ( now < header->getTime ())) {
            this->mHeaderList.clear ();
            return;
        }
        
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
                accepted++;
                break;
            
            case kBlockTreeAppendResult::MISSING_PARENT:
            
                if ( accepted == 0 ) {
                    this->mTag.reset ();
                    this->mImproved.reset ();
                }
                else {
                    this->reset ();
                }
                return;
            
            case kBlockTreeAppendResult::TOO_SOON:
                this->reset ();
                return;
        }
        
        this->mHeaderList.pop_front ();
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
        
        case MiningMessengerRequest::REQUEST_LATEST_HEADERS:
        case MiningMessengerRequest::REQUEST_PREVIOUS_HEADERS: {
                                        
            if ( this->mState == STATE_WAITING_FOR_HEADERS ) {
            
                // sanity check.
                if (( this->mHeaderList.size () && response.mHeaders.size ()) && ( this->mHeaderList.front ()->getHeight () != ( response.mHeaders.back ()->getHeight () + 1 ))) {
                    this->mHeaderList.clear ();
                }
                
                // prepend the new batch of headers and process it.
                this->mHeaderList.insert ( this->mHeaderList.begin (), response.mHeaders.cbegin (), response.mHeaders.cend ());
                this->processHeaders ( miner, now );
                
                this->mState = STATE_ONLINE;
            }
            break;
        }
        
        case MiningMessengerRequest::REQUEST_MINER_INFO: {

            if ( this->mState == STATE_WAITING_FOR_INFO ) {

                this->setMinerID ( response.mMinerID );
                miner.mRemoteMinersByID [ this->getMinerID ()] = this->shared_from_this ();
                
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
    mState ( STATE_OFFLINE ) {
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
            
            if ( this->mHeaderList.size ()) {
            
                LGN_LOG (
                    VOL_FILTER_ROOT,
                    INFO,
                    "%s [%d ... %d]: REWINDING",
                    minerID,
                    ( int )this->mHeaderList.front ()->getHeight (),
                    ( int )this->mHeaderList.back ()->getHeight ()
                );
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

    this->mHeaderList.clear ();

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
        
            if ( this->mHeaderList.size ()) {
                messenger.enqueuePreviousHeadersRequest ( this->mURL, ( *this->mHeaderList.begin ())->getHeight () - 1 );
            }
            else {
                messenger.enqueueLatestHeadersRequest ( this->mURL );
            }
            this->mState = STATE_WAITING_FOR_HEADERS;
            break;
        
        default:
            break;
    }
}

} // namespace Volition
