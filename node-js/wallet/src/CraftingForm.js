/* eslint-disable no-whitespace-before-property */

import { Store, useStore }              from './stores/Store';
import { action, computed, observable } from "mobx";
import { observer }                     from "mobx-react";
import React                            from 'react';
import { Button, Form, List, Segment, Select } from 'semantic-ui-react';

const CLEAR_DROPDOWN_TEXT = '--';

//================================================================//
// CraftingFormValuesStore
//================================================================//
class CraftingFormValuesStore extends Store {

    @observable ingredients = {};
    @observable fieldValues = {};

    //----------------------------------------------------------------//
    constructor ( inventory, methodName ) {
        super ();
        this.initialize ( inventory, methodName );
    }

    //----------------------------------------------------------------//
    @action
    handleChange ( fieldName, value ) {

        const prevValue = this.fieldValues [ fieldName ];
        const valueOrNull = value === CLEAR_DROPDOWN_TEXT ? null : value ;

        if ( prevValue !== null ) {
            this.ingredients [ prevValue ].utilized = false;
        }

        if ( valueOrNull !== null ) {
            this.ingredients [ valueOrNull ].utilized = true;
        }

        this.fieldValues [ fieldName ] = valueOrNull;
    }

    //----------------------------------------------------------------//
    @action
    initialize ( inventory, methodName ) {

        this.paramBindings = inventory.binding.getMethodParamBindings ( methodName );

        for ( let paramName in this.paramBindings ) {

            this.fieldValues [ paramName ] = null;

            const options = this.paramBindings [ paramName ];
            for ( let i in options ) {

                let assetID     = options [ i ];
                let asset       = inventory.assets [ assetID ];

                this.ingredients [ assetID ] = {
                    asset:          inventory.assets [ assetID ],
                    utilized:       false,
                    displayName:    `${ assetID }: ${ asset.fields.displayName || asset.type }`,
                }
            }
        }
    }

    //----------------------------------------------------------------//
    @computed get
    isValid () {

        for ( let fieldName in this.fieldValues ) {
            if ( this.fieldValues [ fieldName ] === null ) {
                return false;
            }
        }
        return true;
    }
}

//================================================================//
// renderFormInputForField
//================================================================//
function renderFormInputForField ( formState, fieldName, options ) {

    let onChange = ( event, payload ) => { formState.handleChange ( fieldName, payload.value )};

    let value = formState.fieldValues [ fieldName ] === null ? '' : formState.fieldValues [ fieldName ];

    const select = [];
    for ( let i in options ) {
        const assetID = options [ i ];
        const ingredient = formState.ingredients [ assetID ];
        const disabled = ingredient.utilized
        select.push ({ key: i, text: ingredient.displayName, value: assetID, disabled: disabled });
    }
    select.push ({ key: options.length, text: CLEAR_DROPDOWN_TEXT, value: CLEAR_DROPDOWN_TEXT });

    return (
        <div key = { fieldName }>
            <Form.Input
                fluid
                control = { Select }
                options = { select }
                placeholder = { fieldName }
                name = { fieldName }
                value = { formState.fieldValues [ fieldName ] || '' }
                onChange = { onChange }
            />
        </div>
    );
}

//================================================================//
// renderFormInputs
//================================================================//
function renderFormInputs ( formState ) {
    // add the fields in order
    const paramBindings = formState.paramBindings;
    let inputs = [];
    for ( let paramName in paramBindings ) {
        let formInput = renderFormInputForField ( formState, paramName, paramBindings [ paramName ]);
        if ( formInput ) {
            inputs.push ( formInput );
        }
    }
    return inputs;
}

//================================================================//
// renderIngredientList
//================================================================//
function renderIngredientList ( formState ) {

    const ingredients = formState.ingredients;
    let ingredientListItem = [];
    for ( let assetID in ingredients ) {
        
        let ingredient = ingredients [ assetID ];

        if  ( !ingredient.utilized ) {
            ingredientListItem.push (<List.Item key = { assetID }>{ ingredient.displayName }</List.Item>);
        }
    }
    return ingredientListItem;
}

//================================================================//
// CraftingForm
//================================================================//
const CraftingForm = observer (( props ) => {

    const formState = useStore (() => new CraftingFormValuesStore ( props.inventory, props.methodName ));

    const onClickSend = () => {
        props.onSubmit ( Object.assign ({}, formState.fieldValues ))
    };

    const inputs = renderFormInputs ( formState );
    const ingredientListItems = renderIngredientList ( formState );
    const isSubmitEnabled = formState.isValid;

    return (
        <Form size = "large">
            <Segment stacked>
                <List>{ ingredientListItems }</List>
                { inputs }
                <Button type = 'button' color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClickSend }>
                    OK
                </Button>
            </Segment>
        </Form>
    );
});

export default CraftingForm;
