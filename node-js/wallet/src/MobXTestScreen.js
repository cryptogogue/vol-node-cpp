/* eslint-disable no-whitespace-before-property */

import { LocalStore, useLocalStore }    from './stores/LocalStore';
import { extendObservable }             from "mobx";
import { observer }                     from "mobx-react";
import React                            from 'react';

//================================================================//
// MobXTestScreen
//================================================================//
class MobXTestScreenStore extends LocalStore {

    //----------------------------------------------------------------//
    constructor () {
        super ();

        this.values = [ 'foo', 'doop', 'moop' ];

        extendObservable ( this, {
            selection: this.values [ 0 ],
        });
    }
}

//================================================================//
// MobXTestScreen
//================================================================//
const MobXTestScreen = observer (( props ) => {

    const store = useLocalStore ( new MobXTestScreenStore ());

    const onKeyDown = ( e ) => {
        const values = store.values
        const idx = values.indexOf ( store.selection )
        if ( e.keyCode === 38 && idx > 0 ) {
            store.selection = values [ idx - 1 ];
        }
        else if ( e.keyCode === 40 && idx < values.length -1 ) {
            store.selection = values [ idx + 1 ];
        }
    }

    const onSelect = ( value ) => {
        console.log ( 'ON SELECT', value );
        store.selection = value
    }

    return (
        <div>
            <ul onKeyDown = {( e ) => onKeyDown ( e )} tabIndex = { 0 }>
                { store.values.map ( value =>
                    <li
                        className   = { value === store.selection ? 'selected' : '' }
                        key         = { value }
                        onClick     = {() => onSelect ( value )}
                    >
                        { value }
                    </li> 
                )}  
            </ul>
            { store.selection }
        </div>
    );
});

export default MobXTestScreen;