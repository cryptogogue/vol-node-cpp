/* eslint-disable no-whitespace-before-property */

import AssetView                        from './AssetView';
import { Service, useService }          from './stores/Service';
import TransactionForm                  from './TransactionForm';
import { TRANSACTION_TYPE }             from './util/Transaction';
import { action, computed, observable } from "mobx";
import { observer }                     from "mobx-react";
import React, { useState }              from 'react';
import { Button, Dropdown, Form, Header, Grid, List, Segment, Select } from 'semantic-ui-react';

const CLEAR_DROPDOWN_TEXT = '--';
const EMPTY_STRING = '';

const CRAFTING_STATE = {
    ASSET_SELECTION:        'ASSET_SELECTION',
    MAKE_TRANSACTION:       'MAKE_TRANSACTION',
};

//================================================================//
// CraftingFormController
//================================================================//
class CraftingFormController extends Service {

    @observable state;

    @observable ingredients = {};    
    @observable fieldValues = {};

    @observable activeField         = EMPTY_STRING;
    @observable activeIngredient    = null;

    @observable ingredientsForField = {};
    @observable fieldsForIngredient = {};

    //----------------------------------------------------------------//
    constructor ( appState, inventory, methodName ) {
        super ();
        this.appState       = appState;
        this.inventory      = inventory;
        this.methodName     = methodName;
        this.reset ();
    }

    //----------------------------------------------------------------//
    decorateTransaction ( transaction ) {

        const method = this.inventory.schema.methods [ this.methodName ];

        const invocation = {
            methodName:         this.methodName,
            assetParams:        Object.assign ({}, this.fieldValues ),
            constParams:        {},
            weight:             method.weight,
            maturity:           method.maturity,
        };

        const body = transaction.body;

        body.invocations = [ invocation ];
        body.weight = invocation.weight;
        body.maturity = invocation.maturity;

        transaction.setAssetsUtilized ( Object.values ( this.fieldValues ));
        transaction.setNote ( this.methodName );
    }

    //----------------------------------------------------------------//
    isActiveField ( fieldName ) {

        return ( this.fieldName === fieldName );
    }

    //----------------------------------------------------------------//
    isActiveIngredient ( assetID ) {

        return (( this.activeIngredient !== null ) && ( this.activeIngredient.assetID === assetID ));
    }

    //----------------------------------------------------------------//
    isEnabledField ( fieldName ) {

        // if there's an active asset, only enable fields that can accept that asset.
        if ( this.activeIngredient !== null ) {

            const methodBinding = this.inventory.getCraftingMethodBindings ()[ this.methodName ];
            return ( methodBinding.assetIDsByArgName [ fieldName ].includes ( this.activeIngredient.assetID ));
        }

        // otherwise, all fields are enabled.
        return true;
    }

    //----------------------------------------------------------------//
    isEnabledIngredient ( assetID ) {

        // if there's an active field, only enable unused ingredients approved for field
        if ( this.activeField !== EMPTY_STRING ) {

            let ingredientsForActiveField = {};
            const options = this.paramBindings [ this.activeField ];
            for ( let i in options ) {
                let assetID = options [ i ];
                ingredientsForActiveField [ assetID ] = this.ingredients [ assetID ];
            }
            return ( assetID in ingredientsForActiveField );
        }

        // otherwise, all ingredients are active
        return true;
    }

    //----------------------------------------------------------------//
    isUtilizedIngredient ( assetID ) {

        return ( this.ingredients [ assetID ].fieldName !== EMPTY_STRING );
    }

    //----------------------------------------------------------------//
    @computed get
    isValid () {

        if ( this.makerKeyName === EMPTY_STRING ) return false;

        for ( let fieldName in this.fieldValues ) {
            if ( this.fieldValues [ fieldName ] === EMPTY_STRING ) {
                return false;
            }
        }
        return true;
    }

    //----------------------------------------------------------------//
    @action
    nextState () {

        if ( this.state === CRAFTING_STATE.ASSET_SELECTION ) {
            this.state = CRAFTING_STATE.MAKE_TRANSACTION;
        }
    }

    //----------------------------------------------------------------//
    @action
    prevState () {

        if ( this.state === CRAFTING_STATE.MAKE_TRANSACTION ) {
            this.state = CRAFTING_STATE.ASSET_SELECTION;
        }
    }

    //----------------------------------------------------------------//
    @action
    select () {

        const fieldName = this.activeField;
        const prevValue = this.fieldValues [ fieldName ];

        if ( prevValue !== EMPTY_STRING ) {
            this.ingredients [ prevValue ].fieldName = EMPTY_STRING;
        }
        this.fieldValues [ fieldName ] = EMPTY_STRING;

        if ( this.activeIngredient !== null ) {
            this.fieldValues [ fieldName ] = this.activeIngredient.assetID;
            this.activeIngredient.fieldName = fieldName;
            this.activeField = EMPTY_STRING;
            this.activeIngredient = null;
        }
    }

    //----------------------------------------------------------------//
    @action
    selectField ( fieldName ) {

        this.activeField = fieldName || EMPTY_STRING;
        this.select ();
    }

    //----------------------------------------------------------------//
    @action
    selectIngredient ( assetID ) {

        let ingredient = assetID ? this.ingredients [ assetID ] : null;

        if ( ingredient.fieldName !== EMPTY_STRING ) {
            this.fieldValues [ ingredient.fieldName ] = EMPTY_STRING;
            ingredient.fieldName = EMPTY_STRING;
        }

        this.activeIngredient = ingredient;
        if ( this.activeField !== EMPTY_STRING ) {
            this.select ();
        }
    }

    //----------------------------------------------------------------//
    @action
    reset () {

        const inventory = this.inventory;
        const methodName = this.methodName;

        this.state = CRAFTING_STATE.ASSET_SELECTION;

        this.paramBindings = inventory.binding.getMethodParamBindings ( methodName );

        for ( let paramName in this.paramBindings ) {

            this.fieldValues [ paramName ] = EMPTY_STRING;

            const options = this.paramBindings [ paramName ];
            for ( let i in options ) {

                let assetID     = options [ i ];
                let asset       = inventory.assets [ assetID ];

                this.ingredients [ assetID ] = {
                    asset:          asset,
                    assetID:        assetID,
                    fieldName:      EMPTY_STRING,
                    displayName:    `${ assetID }: ${ asset.fields.displayName || asset.type }`,
                }
            }
        }
    }
}

//================================================================//
// CraftingFormFieldButtons
//================================================================//
const CraftingFormFieldButtons = observer (( props ) => {

    const [ buttonDOM, setButtonDOM ] = useState ();

    const controller = props.controller;

    // add the fields in order
    const paramBindings = controller.paramBindings;
    let fields = [];
    for ( let fieldName in paramBindings ) {

        let isActive = controller.isActiveField ( fieldName );

        let value = controller.fieldValues [ fieldName ];
        let text = ( value === EMPTY_STRING ) ? fieldName : `${ fieldName }: ${ value }`;

        fields.push (
            <Button
                type = 'button'
                key = { fieldName }
                style = {{
                    outline:    isActive ? 'solid #00FFFF' : '',
                }}
                fluid
                onClick = {() => { controller.selectField ( fieldName )}}
            >
                { text }
            </Button>
        );
    }
    return <div>{ fields }</div>;
});

//================================================================//
// IngredientList
//================================================================//
const IngredientList = observer (( props ) => {

    const controller = props.controller;

    const ingredients = controller.ingredients;
    let ingredientList = [];
    for ( let assetID in ingredients ) {
        
        let ingredient = ingredients [ assetID ];

        const isActive      = controller.isActiveIngredient ( assetID );
        const isEnabled     = controller.isEnabledIngredient ( assetID );
        const isUtilized    = controller.isUtilizedIngredient ( assetID );

        ingredientList.push (
            <div
                key = { assetID }
                style = {{
                    float:      'left',
                    outline:    isActive ? 'thick solid #00FFFF' : ( isUtilized ? 'thick solid #00FF00' : '' ),
                    opacity:    isEnabled ? '1.0' : '0.5',
                }}
                onClick = {() => {
                    isEnabled && controller.selectIngredient ( assetID );
                    console.log ( 'click' );
                }}
            >
                <AssetView
                    inventory = { controller.inventory }
                    assetId = { assetID }
                    inches = 'true'
                />
            </div>
        );
    }
    return <div>{ ingredientList }</div>;
});

//================================================================//
// AssetSelector
//================================================================//
const AssetSelector = observer (( props ) => {

    const { controller } = props;

    const onClick = () => {
        controller.nextState ();
    };

    const isSubmitEnabled = controller.isValid;

    return (
        <div>
            <CraftingFormFieldButtons controller = { controller }/>
            <Button type = 'button' color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClick }>
                OK
            </Button>
        </div>
    );
});

//================================================================//
// CraftingForm
//================================================================//
const CraftingForm = observer (( props ) => {

    const { appState, inventory, methodName } = props;

    const controller = useService (() => new CraftingFormController ( appState, inventory, methodName ));

    const onCancel = () => {
        appState.setNextTransactionCost ( 0 );
        controller.prevState ();
    };

    const onSubmit = ( transaction ) => {
        controller.decorateTransaction ( transaction );
        controller.reset ();
        appState.pushTransaction ( transaction );
    };

    const balance = appState.balance;
    const textColor = balance > 0 ? 'black' : 'red';

    return (
        <div>
            <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                <Grid.Column style = {{ maxWidth: 450 }}>
                    <Segment>
                        <Header as = "h3">
                            { props.methodName }
                        </Header>
                        <Choose >
                            <When condition = { controller.state === CRAFTING_STATE.ASSET_SELECTION }>
                                <AssetSelector controller = { controller }/>
                            </When>
                            <When condition = { controller.state === CRAFTING_STATE.MAKE_TRANSACTION }>
                                <Header as = "h2">
                                    <p style = {{ color: textColor }}>Balance: { balance }</p>
                                </Header>
                                <Segment stacked>
                                    <TransactionForm
                                        appState = { appState }
                                        transactionType = { TRANSACTION_TYPE.RUN_SCRIPT }
                                        onSubmit = { onSubmit }
                                    />
                                </Segment>
                                <Button type = 'button' color = "red" fluid onClick = { onCancel }>
                                    Cancel
                                </Button>
                            </When>
                        </Choose>
                    </Segment>
                </Grid.Column>
            </Grid>
            <IngredientList controller = { controller }/>
        </div>
    );
});

export default CraftingForm;
