/* eslint-disable no-whitespace-before-property */

import { extendObservable, isObservable, observe } from 'mobx';
import { deepObserve }          from 'mobx-utils';
import React                    from 'react';
import { Redirect }             from 'react-router';

//----------------------------------------------------------------//
export function caselessCompare ( a, b ) {
    return (( typeof ( a ) === 'string' ) && ( typeof ( b ) === 'string' )) ?
        ( name.localeCompare ( sheetname, undefined, { sensitivity: 'accent' }) === 0 ) :
        ( a === b );
}

//----------------------------------------------------------------//
export function getAccountId ( props ) {
    let accountID = props.match.params && props.match.params.accountID;
    return accountID && ( accountID.length > 0 ) && accountID;
}

//----------------------------------------------------------------//
export function getMatch ( props, field, fallback ) {
    let match = props.match.params && props.match.params [ field ] || '';
    return match.length > 0 ? match : ( fallback || '' );
}

//----------------------------------------------------------------//
export function getUserId ( props ) {
    return props.match.params && props.match.params.userID;
}

//----------------------------------------------------------------//
export function greater ( x, y ) {
    return x > y ? x : y;
}

//----------------------------------------------------------------//
export function javascriptEscape ( str ) {
    return str
        .replace ( /(\n)/g, `\\n` )
        .replace ( /(\")/g, `\"` )
        .replace ( /(\r)/g, `\\r` )
        .replace ( /(\t)/g, `\\t` )
        .replace ( /(\f)/g, `\\f` );
}

//----------------------------------------------------------------//
export function lesser ( x, y ) {
    return x < y ? x : y;
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

//----------------------------------------------------------------//
export function toNumberOrFalse ( val, types ) {

    types = types || [ 'string', 'number' ];
    return types.includes ( typeof ( val ))  ? Number ( val ) : false;
}

//----------------------------------------------------------------//
export function toStringOrFalse ( val, types ) {

    types = types || [ 'string', 'number' ];
    return types.includes ( typeof ( val ))  ? String ( val ) : false;
}
