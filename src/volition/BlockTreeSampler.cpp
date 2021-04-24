// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/BlockTreeSampler.h>

namespace Volition {

//================================================================//
// BlockTreeSamplerNode
//================================================================//

//----------------------------------------------------------------//
BlockTreeSamplerNode::BlockTreeSamplerNode () {
}

//----------------------------------------------------------------//
BlockTreeSamplerNode::BlockTreeSamplerNode ( const BlockTreeCursor& cursor ) {

    this->mRoot = cursor;
    this->mLeaves.push_back ( cursor );
}

//================================================================//
// BlockTreeSampler
//================================================================//

//----------------------------------------------------------------//
void BlockTreeSampler::addLeaf ( const BlockTreeCursor& leaf ) {

    this->mLeaves.push_back ( leaf );
}

//----------------------------------------------------------------//
BlockTreeSampler::BlockTreeSampler () {
}

//----------------------------------------------------------------//
BlockTreeSampler::~BlockTreeSampler () {
}

//----------------------------------------------------------------//
u64 BlockTreeSampler::findHeight ( const list < BlockTreeSamplerNode >& nodes ) {

    u64 height = 0;

    list < BlockTreeSamplerNode >::const_iterator nodeIt = nodes.cbegin ();
    for ( ; nodeIt != nodes.cend (); ++nodeIt ) {
        u64 nodeHeight = nodeIt->mRoot.getHeight ();
        if ( height < nodeHeight ) {
            height = nodeHeight;
        }
    }
    return height;
}

//----------------------------------------------------------------//
BlockTreeSamplerNode BlockTreeSampler::sample ( double threshold ) const {
    UNUSED ( threshold );
    
    size_t minSize = ( size_t )floor ( this->mLeaves.size () * threshold );
    
    list < BlockTreeSamplerNode > nodes;
    
    list < BlockTreeCursor >::const_iterator leafIt = this->mLeaves.cbegin ();
    for ( ; leafIt != this->mLeaves.cend (); ++leafIt ) {
        nodes.push_back ( BlockTreeSamplerNode ( *leafIt ));
    }
    
    // find the height of the top node(s)
    u64 height = this->findHeight ( nodes );
    
    do {
    
        // iterate through the list of nodes and merge
        list < BlockTreeSamplerNode >::iterator outerIt = nodes.begin ();
        for ( ; outerIt != nodes.end (); ++outerIt ) {
        
            BlockTreeSamplerNode& node = *outerIt;
        
            // only consider nodes at the current height
            if ( node.mRoot.getHeight () != height ) continue;
                        
            // check all subsequent nodes
            list < BlockTreeSamplerNode >::iterator innerIt = std::next ( outerIt, 1 );
            while ( innerIt != nodes.end ()) {
            
                list < BlockTreeSamplerNode >::iterator cursor = innerIt++;
            
                // if the node matches the root, merge its leaves and remove it from the list
                if ( cursor->mRoot.equals ( node.mRoot )) {
                    node.mLeaves.splice ( node.mLeaves.end (), cursor->mLeaves );
                    node.mChildren.splice ( node.mChildren.end (), cursor->mChildren );
                    nodes.erase ( cursor );
                }
            }
            
            if ( outerIt->mLeaves.size () > minSize ) return *outerIt;
            
            // advance the root to its parent
            if ( height > 0 ) {
                node.mChildren.clear ();
                node.mChildren.push_back ( node.mRoot );
                outerIt->mRoot = outerIt->mRoot.getParent ();
            }
        }
        
    } while (( height > 0 ) && height-- );
    
    assert ( false );
    return BlockTreeSamplerNode ();
}

} // namespace Volition
