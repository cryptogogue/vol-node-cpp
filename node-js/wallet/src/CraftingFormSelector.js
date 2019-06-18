/* eslint-disable no-whitespace-before-property */

import { withAppState }                 from './AppStateProvider';
import CraftingForm                     from './CraftingForm';
import { observer }                     from "mobx-react";
import React, { useState }              from 'react';
import { Button, Divider, Dropdown }    from 'semantic-ui-react';

//================================================================//
// CraftingFormSelector
//================================================================//
const CraftingFormSelector = observer (( props ) => {

    const [ formIsVisible, setFormIsVisible ]   = useState ( false );
    const [ methodIndex, setMethodIndex ]       = useState ( -1 );

    const { inventory } = props;

    const methods = inventory.validMethods;
    const hasMethods = ( methods.length > 0 );

    const handleSubmit = ( fieldValues ) => {
        // fieldValues.makerNonce = this.props.nonce;
        // this.showForm ( false );
        // this.props.appState.startTransaction ( schema, fieldValues );
    }

    const showForm = ( show ) => {
        setFormIsVisible ( show );
        setMethodIndex ( -1 );
    }

    if ( hasMethods && formIsVisible ) {

        const methodName = methodIndex >= 0 ? methods [ methodIndex ] : '';

        let dropdownOptions = [];
        for ( let i in methods ) {
            dropdownOptions.push ({ key:methods [ i ], value:i, text:methods [ i ]});
        }

        return (
            <div>
                <Dropdown
                    placeholder = "Select Action"
                    fluid
                    search
                    selection
                    options = { dropdownOptions }
                    onChange = {( event, data ) => { setMethodIndex ( data.value )}}
                />
                <If condition = { methodName.length > 0 }>
                    <CraftingForm
                        key         = { methodName }
                        methodName  = { methodName }
                        inventory   = { inventory }
                        onSubmit    = {( fieldValues ) => { handleSubmit ( fieldValues )}}
                    />
                </If>
                <Divider/>
                <Button color = "red" fluid onClick = {() => { showForm ( false )}}>
                    Cancel
                </Button>
            </div>
        );
    }

    return (
        <div>
            <Button color = "teal" fluid disabled = { !hasMethods } onClick = {() => { showForm ( true )}}>
                New Action
            </Button>
        </div>
    );
});

export default CraftingFormSelector;
