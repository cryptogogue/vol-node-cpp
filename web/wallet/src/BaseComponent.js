/* eslint-disable no-whitespace-before-property */

import React, { Component }     from 'react';
import { Redirect }             from 'react-router-dom';

//================================================================//
// BaseComponent
//================================================================//
class BaseComponent extends Component {

    //----------------------------------------------------------------//
    componentWillUnmount () {

        this.revoked = true;

        this.revocables.forEach (( revoke ) => {
            revoke ();
        });
        this.revocables.clear ();
    }

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.revocables = new Map (); // need to use a propet set to contain objects
        this.revoked = false;
    }

    //----------------------------------------------------------------//
    isRevoked () {
        return this.revoked;
    }

    //----------------------------------------------------------------//
    prefixURL ( url ) {

        let userId = this.props.match.params.userId;
        if ( userId && userId.length ) {
            return '/' + userId + url;
        }
        return url;
    }

    //----------------------------------------------------------------//
    redirect ( url ) {
        console.log ( 'REDIRECT:', this.prefixURL ( url ));
        return (<Redirect to = { this.prefixURL ( url )}/>);
    }

    //----------------------------------------------------------------//
    revocableAll ( promises ) {
        return this.revocablePromise ( Promise.all ( promises ));
    }

    //----------------------------------------------------------------//
    revocableFetch ( input, init ) {
        return this.revocablePromise ( fetch ( input, init ));
    }

    //----------------------------------------------------------------//
    revocableFetchJSON ( input, init ) {
        return this.revocableFetch ( input, init )
                .then ( response => this.revocablePromise ( response.json ()));
    }

    //----------------------------------------------------------------//
    revocablePromise ( promise ) {

        let isCancelled = false;

        const wrappedPromise = new Promise (( resolve, reject ) => {

            let onFulfilled = ( value ) => {
                if ( isCancelled ) {
                    reject ({ isCanceled: true });
                }
                else {
                    resolve ( value );
                }
            }

            let onRejected = ( error ) => {
                if ( isCancelled ) {
                    reject ({ isCanceled: true });
                }
                else {
                    reject ( error );
                }
            }

            let onFinally = () => {
                this.revocables.delete ( wrappedPromise );
            }

            promise.then ( onFulfilled, onRejected )
            .finally ( onFinally );
        });

        this.revocables.set ( wrappedPromise, () => {
            isCancelled = true
            console.log ( 'AUTO-REVOKED PROMISE!' );
        });
        return wrappedPromise;
    };

    //----------------------------------------------------------------//
    revocableSetState ( value ) {
        if ( !this.revoked ) {
            this.setState ( value );
        }
    }

    //----------------------------------------------------------------//
    revocableTimeout ( callback, delay ) {
        
        if ( this.revoked ) return;

        let timeout = setTimeout (() => {
            this.revocables.delete ( timeout );
            callback ();
        }, delay );

        this.revocables.set ( timeout, () => {
            clearTimeout ( timeout );
            console.log ( 'AUTO-REVOKED TIMEOUT!' );
        });
        return timeout;
    }
}

export default BaseComponent