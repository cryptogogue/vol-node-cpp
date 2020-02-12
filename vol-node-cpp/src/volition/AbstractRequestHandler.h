// Copyright (c) 2017-2018 Cryptogogue, Inc. All Rights Reserved.
// http://cryptogogue.com

#ifndef VOLITION_ABSTRACTREQUESTHANDLER_H
#define VOLITION_ABSTRACTREQUESTHANDLER_H

#include <volition/common.h>

namespace Volition {

//================================================================//
// AbstractRequestHandler
//================================================================//
class AbstractRequestHandler :
    public Poco::Net::HTTPRequestHandler {
private:
    
    unique_ptr < Routing::PathMatch > mMatch;

    //----------------------------------------------------------------//
    void            handleRequest                           ( Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response ) override;

protected:

    //----------------------------------------------------------------//
    virtual void    AbstractRequestHandler_handleRequest    ( const Routing::PathMatch& match, Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response ) const = 0;

public:

    //----------------------------------------------------------------//
                    AbstractRequestHandler                  ();
                    ~AbstractRequestHandler                 ();
    string          getMatchString                          ( string key ) const;
    u64             getMatchU64                             ( string key ) const;
    u64             optMatch                                ( string key, u64 fallback ) const;
    string          optMatch                                ( string key, string fallback ) const;
    void            setMatch                                ( const Routing::PathMatch& match );
};

} // namespace Volition
#endif
