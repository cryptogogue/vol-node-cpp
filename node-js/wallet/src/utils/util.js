/* eslint-disable no-whitespace-before-property */

import { extendObservable, isObservable, observe } from 'mobx';
import { deepObserve }          from 'mobx-utils';
import React                    from 'react';
import { Redirect }             from 'react-router';

//----------------------------------------------------------------//
export function getAccountId ( props ) {
    let accountId = props.match.params && props.match.params.accountId;
    return accountId && ( accountId.length > 0 ) && accountId;
}

//----------------------------------------------------------------//
export function getUserId ( props ) {
    return props.match.params && props.match.params.userId;
}

//----------------------------------------------------------------//
export function observeField ( container, field, callback ) {

    let valueDisposer;

    const setValueObserver = () => {

        valueDisposer && valueDisposer (); // not strictly necessary, but why not?

        if ( isObservable ( container [ field ])) {
            valueDisposer = deepObserve ( container [ field ], callback );
        }
    }

    setValueObserver ();

    let fieldDisposer = observe ( container, field, ( change ) => {
        setValueObserver ();
        callback ( change );
    });

    return () => {
        fieldDisposer ();
        valueDisposer && valueDisposer ();
    }
}
