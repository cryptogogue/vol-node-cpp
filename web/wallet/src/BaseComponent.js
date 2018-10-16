/* eslint-disable no-whitespace-before-property */

import { Component }     from 'react';

//================================================================//
// BaseComponent
//================================================================//
class BaseComponent extends Component {

    //----------------------------------------------------------------//
    componentWillUnmount () {
        this.revocables.forEach (( revoke ) => {
            revoke ();
        });
        this.revocables.clear ();
    }

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.revocables = new Map (); // need to use a propet set to contain objects
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
        return promise;
    };

    //----------------------------------------------------------------//
    revocableTimeout ( callback, delay ) {
        
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