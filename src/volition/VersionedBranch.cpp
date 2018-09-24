// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#include <volition/State.h>
#include <volition/VersionedStoreSnapshot.h>
#include <volition/VersionedBranch.h>

namespace Volition {

//================================================================//
// VersionedBranch
//================================================================//

//----------------------------------------------------------------//
/** \brief Returns the total number of dependencies (i.e. clients).

    \return     Total clients.
*/
size_t VersionedBranch::countDependencies () const {

    return this->mClients.size ();
}

//----------------------------------------------------------------//
/** \brief Removes the client from the branch's client set.

    \param      client      Client to erase.
*/
void VersionedBranch::eraseClient ( AbstractVersionedBranchClient& client ) {

    this->mClients.erase ( &client );
}

//----------------------------------------------------------------//
/** \brief Iterates through all clients to find the top immutable version
    in the branch. All versions less than the top must not be changed.
 
    The "immutable top" is the top of versions in the branch considered
    immutable. A version is immutable if a client depends on it.
 
    The "ignore" parameter is provided to handle the case where a
    single client wishes to change the contents of a version. If the
    version only has the client as a dependent, then it is safe to
    change (without creating a new branch).

    \param      ignore      Client to ignore (or NULL).
    \return                 The upper limit of immutable versions.
*/
size_t VersionedBranch::findImmutableTop ( const AbstractVersionedBranchClient* ignore ) const {

    LOG_SCOPE_F ( INFO, "VersionedBranch::findImmutableTop ()" );

    size_t immutableTop = this->getVersionDependency ();

    set < AbstractVersionedBranchClient* >::const_iterator clientIt = this->mClients.cbegin ();
    for ( ; clientIt != this->mClients.cend (); ++clientIt ) {

        const AbstractVersionedBranchClient* client = *clientIt;
        if ( client != ignore ) {
        
            size_t clientVersion = client->getVersionDependency ();
            
            if ( immutableTop < clientVersion ) {
                immutableTop = clientVersion;
            }
        }
    }
    
    LOG_F ( INFO, "immutableTop: %d", ( int )immutableTop );
    
    return immutableTop;
}

//----------------------------------------------------------------//
/** \brief Returns the ValueStack corresponding to the given key, if it
    exists in the branch. Does not recurse: searches only the local branch.

    \param      key         The key.
    \return                 The ValueStack for the key or NULL.
*/
const AbstractValueStack* VersionedBranch::findValueStack ( string key ) const {

    map < string, unique_ptr < AbstractValueStack >>::const_iterator valueIt = this->mValueStacksByKey.find ( key );
    return ( valueIt != this->mValueStacksByKey.cend ()) ? valueIt->second.get () : NULL;
}

//----------------------------------------------------------------//
/** \brief Initializes a new branch using the given branch as a parent. If the base
    version is less than the top of the parent branch, then the corresponding layer is copied.
 
    A parent branch must be provided. If the base version given is below the parent's immutable
    top (i.e. other clients or children depend on later versions) then all values set in that
    version must be copied into the child branch.
 
    The given version must be greated than or equal to the base version of the parent branch. (We
    could iterate back through the list of branches if an earlier version is given, but for the
    use case of this constructor, that functionality isn't needed.)
 
    There is a special case when a child branch is created with a base version of zero. Since
    there are no earlier versions, the parent branch's version may still be copied from, but no
    parent is set in the child. The child becomes a new root and thus has no parents.
 
    \param      parent          Parent branch.
    \param      baseVersion     Base version of the child branch.
*/
shared_ptr < VersionedBranch > VersionedBranch::fork ( size_t baseVersion ) {
    
    shared_ptr < VersionedBranch > child = make_shared < VersionedBranch >();
    
    assert (( this->mVersion <= baseVersion ) && ( baseVersion <= this->getTopVersion ()));

    child->setBranch ( this->mVersion < baseVersion ? this->shared_from_this () : this->mSourceBranch );
    child->mVersion = baseVersion;
    
    map < size_t, Layer >::const_iterator layerIt = this->mLayers.find ( baseVersion );
    if ( layerIt != this->mLayers.cend ()) {
    
        const Layer& fromLayer = layerIt->second;
        Layer& toLayer = child->mLayers [ baseVersion ];
        
        Layer::const_iterator keyIt = fromLayer.cbegin ();
        for ( ; keyIt != fromLayer.cend (); ++keyIt ) {
            
            toLayer.insert ( *keyIt );
            
            const AbstractValueStack* fromStack = this->findValueStack ( *keyIt );
            assert ( fromStack );
            
            unique_ptr < AbstractValueStack >& toStack = child->mValueStacksByKey [ *keyIt ];
            if ( !toStack ) {
                toStack = fromStack->makeEmptyCopy ();
            }
            toStack->copyValueFrom ( *fromStack, baseVersion );
        }
    }
    return child;
}

//----------------------------------------------------------------//
/** \brief Returns the top of the version stack or 0 if the stack is empty.

    \return     The top of the version stack or 0 if the branch is empty.
*/
size_t VersionedBranch::getTopVersion () const {

    // If there are any layers, use the top layer's version. Otherwise, 0.
    return this->mLayers.size () ? this->mLayers.rbegin ()->first + 1 : 0;
}

//----------------------------------------------------------------//
/** \brief Inserts a client into the branch's client set. Inserting a client
    adds a dependency on a specific layer in the branch.
*/
void VersionedBranch::insertClient ( AbstractVersionedBranchClient& client ) {

    this->mClients.insert ( &client );
}

//----------------------------------------------------------------//
/** \brief Attempts to optimize the branch.

    "Optimizing" a branch means two things: trim any unused versions from the
    top of the branch, and concatenate the longest child branch if the child
    branches from the top of the current branch.
 
    Unused versions are simply those equal to or greatet than the "immutable top"
    version. The immutable top can fall below the top version following the
    removal of the top client. In this case, the branch can be truncated.
 
    Following truncation, if there are child branches with their bases at the
    top pf the branch, one of those children may be joined to the branch. In
    this case, we select the longest child branch. When the child is joined to
    the parent, all of its clients must also be moved to the parent.
 
    Optimization is performed recursively on any child branch selected to be
    be joined to the parent.
 
    The presence of "direct references" will prevent the join optimization.
    A "direct reference" is a pointer to a branch internal, such as a value.
    As the join operation may invalidate internals, it must be prevented as
    long as direct references exist. This is accomplished using a reference
    count maintained by the VersionedValue object.
*/
void VersionedBranch::optimize () {

    LOG_SCOPE_F ( INFO, "VersionedBranch::optimize ()" );
    
    // use one loop to find the immutable top and to identify a child branch that
    // may be joined to the parent branch.
    
    size_t immutableTop = this->mVersion; // immutable top won't be less than the branch's base version.
    bool preventJoin = this->preventJoin (); // don't allow join if there are any direct references to the current branch. (May be over-cautious.)
    AbstractVersionedBranchClient* bestJoin = NULL; // a join will be performed if this is non-NULL.
    
    // loop through every client...
    LOG_F ( INFO, "evaluating clients for possible concatenation..." );
    set < AbstractVersionedBranchClient* >::const_iterator clientIt = this->mClients.cbegin ();
    for ( ; clientIt != this->mClients.cend (); ++clientIt ) {

        AbstractVersionedBranchClient* client = *clientIt;
        LOG_SCOPE_F ( INFO, "client %p", client );
        
        size_t clientVersion = client->getVersionDependency (); // store the client's version dependency to avoid extra function calls.
        
        // the immutable top is the *highest* client version dependency. update it if and only it
        // a client with a higher depenency is found.
        if ( immutableTop < clientVersion ) {
            immutableTop = clientVersion;
            
            // if we've found a more recent client, invalidate any candidate for joining.
            // we can only optimize by joining a child branch located at the top of
            // the branch.
            bestJoin = NULL;
        }
        
        // if nothing is preventing a join, check to see if we can (and should) select
        // the current client as our new candidate for join.
        if (( !preventJoin ) && client->canJoin ()) {

            // a client branch with direct refereces will prevent any join.
            if ( client->preventJoin ()) {
                preventJoin = true; // stop considering join candidates.
                bestJoin = NULL; // clear out any existing join candidate.
            }
            else {
            
                // the "join score" is just the length of the branch. if we don't yet have a join
                // candidate, pick the current client. if we already have a candidate, pick the
                // one with the higher join score.
                if (( !bestJoin ) || ( bestJoin->getJoinScore () < client->getJoinScore ())) {
                    LOG_F ( INFO, "found a client that can join" );
                    bestJoin = client;
                }
            }
        }
    }
    
    LOG_F ( INFO, "immutableTop: %d", ( int )immutableTop );
    LOG_F ( INFO, "topVersion: %d", ( int )this->getTopVersion ());
    
    // throw away any versions equal to or greated than the immutable top.
    this->truncate ( immutableTop );
    
    // if we have a join candidate, perform the join.
    if ( bestJoin ) {
        assert ( bestJoin->getVersionDependency () >= immutableTop );
        bestJoin->joinBranch ( *this );
    }
}

//----------------------------------------------------------------//
/** \brief Discards all layers and values with versions greater than or equal to the
    given version.
 
    \param      topVersion  Version for new "top" of branch.
*/
void VersionedBranch::truncate ( size_t topVersion ) {

    LOG_SCOPE_F ( INFO, "truncate: %d -> %d", ( int )this->getTopVersion (), ( int )topVersion );

    map < size_t, Layer >::reverse_iterator layerIt = this->mLayers.rbegin ();
    while (( layerIt != this->mLayers.rend ()) && ( layerIt->first >= topVersion )) {
    
        LOG_SCOPE_F ( INFO, "popping layer: %d", ( int )layerIt->first );
        
        Layer& layer = layerIt->second;
        
        Layer::iterator keyIt = layer.begin ();
        for ( ; keyIt != layer.end (); ++keyIt ) {

            unique_ptr < AbstractValueStack >& valueStack = this->mValueStacksByKey [ *keyIt ];
            assert ( valueStack );
            
            valueStack->erase ( layerIt->first );
            if ( valueStack->size () == 0 ) {
                this->mValueStacksByKey.erase ( *keyIt );
            }
        }
        this->mLayers.erase ( layerIt->first );
        layerIt = this->mLayers.rbegin ();
    }
}

//----------------------------------------------------------------//
VersionedBranch::VersionedBranch () :
    mDirectReferenceCount ( 0 ) {
}

//----------------------------------------------------------------//
/** \brief Asserts that no direct references remain.
*/
VersionedBranch::~VersionedBranch () {

    assert ( this->mDirectReferenceCount == 0 );
}

//================================================================//
// overrides
//================================================================//

//----------------------------------------------------------------//
/** \brief Implementation of virtual method. Always returns true.
    \return     Always returns true.
*/
bool VersionedBranch::AbstractVersionedStoreClient_canJoin () const {
    return true;
}

//----------------------------------------------------------------//
/** \brief Implementation of virtual method. Returns the top version.
    \return     The top version.
*/
size_t VersionedBranch::AbstractVersionedStoreClient_getJoinScore () const {
    return this->getTopVersion ();
}

//----------------------------------------------------------------//
/** \brief Implementation of virtual method. Returns the base version;
    the branch depends on all versions less than the base version.
 
    \return     The base version.
*/
size_t VersionedBranch::AbstractVersionedStoreClient_getVersionDependency () const {
    return this->mVersion;
}

//----------------------------------------------------------------//
/** \brief Implementation of virtual method. Appends the contents of
    the branch onto the given branch and transfers all clients and
    children to the given branch.
 
    The branch is optimized before being appended. Optimization may
    recursively trigger additional joins.
 
    Neither branch is permitted to have direct references.
 
    \param      branch      The branch to be appended to.
*/
void VersionedBranch::AbstractVersionedStoreClient_joinBranch ( VersionedBranch& branch ) {

    assert ( branch.mDirectReferenceCount == 0 );
    assert ( this->mDirectReferenceCount == 0 );

    LOG_SCOPE_F ( INFO, "VersionedBranch::AbstractVersionedStoreClient_joinBranch ()" );
    LOG_F ( INFO, "JOINING PARENT BRANCH" );
    
    this->optimize ();
    
    shared_ptr < VersionedBranch > pinThis = this->shared_from_this ();
    
    // merge the branch layers
    branch.mLayers.insert ( this->mLayers.begin(), this->mLayers.end ());

    // copy the value stacks
    map < string, unique_ptr < AbstractValueStack >>::iterator valueStackIt = this->mValueStacksByKey.begin ();
    for ( ; valueStackIt != this->mValueStacksByKey.end (); ++valueStackIt ) {
        
        const AbstractValueStack* fromStack = this->findValueStack ( valueStackIt->first );
        assert ( fromStack );
        
        unique_ptr < AbstractValueStack >& toStack = branch.mValueStacksByKey [ valueStackIt->first ];
        if ( !toStack ) {
            toStack = fromStack->makeEmptyCopy ();
        }
        toStack->copyFrom ( *fromStack );
    }

    // copy the clients
    set < AbstractVersionedBranchClient* >::iterator clientIt = this->mClients.begin ();
    for ( ; clientIt != this->mClients.end (); ++clientIt ) {
        AbstractVersionedBranchClient* client = *clientIt;
        branch.insertClient ( *client );
        client->mSourceBranch = branch.shared_from_this ();
    }
    
    pinThis = NULL;
}

//----------------------------------------------------------------//
/** \brief Implementation of virtual method. Prevents a join optimization
    from happening if the branch has any direct references.
 
    \return     True if the branch has any direct references. False otherwise.
*/
bool VersionedBranch::AbstractVersionedStoreClient_preventJoin () const {
    return ( this->mDirectReferenceCount > 0 );
}

} // namespace Volition
