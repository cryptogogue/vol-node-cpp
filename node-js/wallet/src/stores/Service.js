/* eslint-disable no-whitespace-before-property */

import * as util        from '../util/util';
import { observe }      from 'mobx';
import React            from 'react';
import { Redirect }     from 'react-router-dom';

//================================================================//
// Store
//================================================================//
export class Service {

    //----------------------------------------------------------------//
    constructor () {

        this.disposeObservers ();

        this.revocables = new Map (); // need to use a propet set to contain objects
        this.revoked = false;
    }

    //----------------------------------------------------------------//
    disposeObservers () {

        if ( this.observerDisposers ) {

            for ( let key in this.observerDisposers ) {
                this.observerDisposers [ key ]();
            }
        }
        this.observerDisposers = {};
    }

    //----------------------------------------------------------------//
    isRevoked () {
        return this.revoked;
    }

    //----------------------------------------------------------------//
    observeMember ( name, callback ) {

        this.observerDisposers [ name ] = util.observeField ( this, name, callback );
    }

    //----------------------------------------------------------------//
    revocableAll ( promises ) {
        return this.revocablePromise ( Promise.all ( promises ));
    }

    //----------------------------------------------------------------//
    revocableFetch ( input, init, timeout ) {
        return this.revocablePromise ( fetch ( input, init ), timeout || 1000 );
    }

    //----------------------------------------------------------------//
    revocableFetchJSON ( input, init, timeout ) {
        return this.revocableFetch ( input, init, timeout || 1000 )
            .then ( response => this.revocablePromise ( response.json ()));
    }

    //----------------------------------------------------------------//
    revocablePromise ( promise, timeout ) {

        let isCancelled = false;
        let timer;

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
                clearTimeout ( timer );
                this.revocables.delete ( wrappedPromise );
            }

            if ( timeout ) {
                timer = setTimeout (() => {
                    onRejected ( 'TIMED OUT' );
                }, timeout );
            }

            promise.then ( onFulfilled, onRejected )
            .finally ( onFinally );
        });

        this.revocables.set ( wrappedPromise, () => {
            isCancelled = true
            clearTimeout ( timer );
            console.log ( 'REVOKED PROMISE!' );
        });
        return wrappedPromise;
    };

    //----------------------------------------------------------------//
    revocablePromiseWithBackoff ( makePromise, wait, asService, step, max, retries ) {

        step = step || 2;
        retries = retries || 0;
        max = typeof ( max ) == 'number' ? ( max > 0 ? max : false ) : wait * 10;

        this.revocablePromise ( makePromise ())

        .then (() => {
            if ( asService ) {
                this.revocableTimeout (() => { this.revocablePromiseWithBackoff ( makePromise, wait, asService, step, max )}, wait );
            }
        })
        
        .catch (( error ) => {

            console.log ( error );
            
            retries = retries + 1;
            let retryDelay = wait * Math.pow ( 2, retries );
            retryDelay = ( max && ( retryDelay < max )) ? retryDelay : max;

            console.log ( 'RETRY:', retries, retryDelay );

            this.revocableTimeout (() => { this.revocablePromiseWithBackoff ( makePromise, wait, asService, step, max, retries )}, retryDelay );
        },

        wait );
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

    //----------------------------------------------------------------//
    revoke ( revocable ) {

        if ( map.has ( revocable )) {
            map [ revocable ]();
            map.delete ( revocable );
        }
    }

    //----------------------------------------------------------------//
    revokeAll () {

        this.revocables.forEach (( revoke ) => {
            revoke ();
        });
        this.revocables.clear ();
    }

    //----------------------------------------------------------------//
    shutdown () {

        this.revoked = true;
        this.revokeAll ();
    }
}

//================================================================//
// hooks
//================================================================//

//----------------------------------------------------------------//
export function useService ( factory ) {

    const serviceRef = React.useRef ();
    serviceRef.current = serviceRef.current || factory ();

    React.useEffect (
        () => {

            const current = serviceRef.current;

            return () => {
                if ( current.shutdown ) {
                    current.shutdown ();
                }
            };
        },
        []
    );

    return serviceRef.current;
}
