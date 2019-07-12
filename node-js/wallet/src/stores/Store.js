/* eslint-disable no-whitespace-before-property */

import * as util        from '../utils/util';
import { observe }      from 'mobx';
import React            from 'react';
import { Redirect }     from 'react-router-dom';

//================================================================//
// Store
//================================================================//
export class Store {

    //----------------------------------------------------------------//
    constructor () {
        this.disposeObservers ();
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
    observeMember ( name, callback ) {

        this.observerDisposers [ name ] = util.observeField ( this, name, callback );
    }

    //----------------------------------------------------------------//
    shutdown () {
    }
}

//================================================================//
// hooks
//================================================================//

//----------------------------------------------------------------//
export function useStore ( factory ) {

    const storeRef = React.useRef ();
    storeRef.current = storeRef.current || factory ();

    React.useEffect (
        () => {

            const current = storeRef.current;

            return () => {
                if ( current.shutdown ) {
                    current.shutdown ();
                }
            };
        },
        []
    );

    return storeRef.current;
}
