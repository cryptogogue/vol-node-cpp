/* eslint-disable no-whitespace-before-property */

import { Store }        from './Store';

//================================================================//
// Store
//================================================================//
export class Service extends Store {

    //----------------------------------------------------------------//
    constructor () {
        super ();

        this.revocables = new Map (); // need to use a propet set to contain objects
        this.revoked = false;
    }

    //----------------------------------------------------------------//
    isRevoked () {
        return this.revoked;
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
                    console.log ( 'HERE', error );
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
            console.log ( 'REVOKED PROMISE!' );
        });
        return wrappedPromise;
    };

    //----------------------------------------------------------------//
    revocablePromiseWithBackoff ( makePromise, wait, step, asService, retries ) {

        step = step || 2;
        retries = retries || 0;

        this.revocablePromise ( makePromise ())
            .then (() => {
                if ( asService ) {
                    this.revocableTimeout (() => { this.revocablePromiseWithBackoff ( makePromise, wait, step, asService )}, wait );
                }
            })
            .catch (( error ) => {

                console.log ( error );
                
                retries = retries + 1;
                let retryDelay = wait * Math.pow ( 2, retries );
                console.log ( 'RETRY:', retries, retryDelay );

                this.revocableTimeout (() => { this.revocablePromiseWithBackoff ( makePromise, wait, step, asService, retries )}, retryDelay );
            })
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
    shutdownAndRevokeAll () {

        this.revoked = true;
        this.revokeAll ();
        super.shutdown ();
    }
}
