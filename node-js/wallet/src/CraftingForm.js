/* eslint-disable no-whitespace-before-property */

import AssetView                        from './AssetView';
import { Service, useService }          from './stores/Service';
import { action, computed, observable } from "mobx";
import { observer }                     from "mobx-react";
import React, { useState }              from 'react';
import { Button, Dropdown, Form, Header, Grid, List, Segment, Select } from 'semantic-ui-react';

const CLEAR_DROPDOWN_TEXT = '--';
const EMPTY_STRING = '';

//================================================================//
// CraftingFormController
//================================================================//
class CraftingFormController extends Service {

    @observable ingredients = {};    
    @observable fieldValues = {};

    @observable activeField         = EMPTY_STRING;
    @observable activeIngredient    = null;

    @observable ingredientsForField = {};
    @observable fieldsForIngredient = {};

    //----------------------------------------------------------------//
    constructor ( inventory, methodName ) {
        super ();
        this.inventory = inventory;
        this.methodName = methodName;
        this.initialize ( inventory, methodName );
    }

    //----------------------------------------------------------------//
    @action
    initialize ( inventory, methodName ) {

        this.paramBindings = inventory.binding.getMethodParamBindings ( methodName );

        for ( let paramName in this.paramBindings ) {

            this.fieldValues [ paramName ] = EMPTY_STRING;

            const options = this.paramBindings [ paramName ];
            for ( let i in options ) {

                let assetID     = options [ i ];
                let asset       = inventory.assets [ assetID ];

                this.ingredients [ assetID ] = {
                    asset:          inventory.assets [ assetID ],
                    assetID:        assetID,
                    fieldName:      EMPTY_STRING,
                    displayName:    `${ assetID }: ${ asset.fields.displayName || asset.type }`,
                }
            }
        }
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

        for ( let fieldName in this.fieldValues ) {
            if ( this.fieldValues [ fieldName ] === EMPTY_STRING ) {
                return false;
            }
        }
        return true;
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

        ingredientList.push (<AssetView
            key = { assetID }
            style = {{
                float:      'left',
                outline:    isActive ? 'thick solid #00FFFF' : ( isUtilized ? 'thick solid #00FF00' : '' ),
                opacity:    isEnabled ? '1.0' : '0.5',
            }}
            inventory = { controller.inventory }
            assetId = { assetID }
            onClick = {() => { isEnabled && controller.selectIngredient ( assetID )}}
        />);
    }
    return <div>{ ingredientList }</div>;
});

//================================================================//
// CraftingForm
//================================================================//
const CraftingForm = observer (( props ) => {

    const controller = useService (() => new CraftingFormController ( props.inventory, props.methodName ));

    const onClickSend = () => {
        props.onSubmit ( Object.assign ({}, controller.fieldValues ))
    };

    const isSubmitEnabled = controller.isValid;

    return (
        <div>
            <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                <Grid.Column style = {{ maxWidth: 450 }}>
                    <Segment>
                        <Header as = "h3">
                            { props.methodName }
                        </Header>
                        <CraftingFormFieldButtons controller = { controller }/>
                        <Button type = 'button' color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClickSend }>
                            OK
                        </Button>
                    </Segment>
                </Grid.Column>
            </Grid>
            <IngredientList controller = { controller }/>
        </div>
    );
});

export default CraftingForm;
