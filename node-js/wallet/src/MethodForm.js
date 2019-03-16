/* eslint-disable no-whitespace-before-property */

import { withAppState }             from './AppStateProvider';
import React, { Component }         from 'react';
import { Button, Form, List, Segment, Select } from 'semantic-ui-react';

const ASSET_FORM_FIELD_TYPE = 'asset';
const CLEAR_DROPDOWN_TEXT = '--';

//================================================================//
// MethodForm
//================================================================//
class MethodForm extends Component {

    //----------------------------------------------------------------//
    checkFormInputs () {

        // const { schema } = this.props;
        // let isValid = true;

        // Object.keys ( schema.fields ).forEach (( fieldName ) => {
        //     let fieldValue = this.state [ fieldName ];
        //     if ( fieldValue === null ) {
        //         isValid = false;
        //     }
        // });

        // return isValid;
        return true;
    }

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        const { inventory, methodName } = this.props;

        this.formFields = inventory.getMethodFormFields ( methodName );

        let assetSet = {};
        let fieldValues = {};
        for ( let fieldName in this.formFields ) {

            fieldValues [ fieldName ] = null;

            const formField = this.formFields [ fieldName ];
            if ( formField.type === ASSET_FORM_FIELD_TYPE ) {
                for ( let i in formField.options ) {
                    assetSet [ formField.options [ i ]] = true;
                }
            }
        }

        this.ingredients = {};
        for ( let assetName in assetSet ) {
            let asset = inventory.assets [ assetName ];
            this.ingredients [ assetName ] = {
                quantity:   asset.quantity,
                utilized: 0,
            };
        }

        this.state = fieldValues;
    }

    //----------------------------------------------------------------//
    handleChange ( fieldName, value ) {

        const prevValue = this.state [ fieldName ];
        const valueOrNull = value === CLEAR_DROPDOWN_TEXT ? null : value ;

        if ( prevValue !== null ) {
            this.ingredients [ prevValue ].utilized--;
        }

        if ( valueOrNull !== null ) {
            this.ingredients [ valueOrNull ].utilized++;
        }

        this.setState ({[ fieldName ]: valueOrNull });
    }

    //----------------------------------------------------------------//
    makeFormInputForField ( fieldName, formField ) {

        let onChange = ( event, payload ) => { this.handleChange ( fieldName, payload.value )};

        let value = this.state [ fieldName ] === null ? '' : this.state [ fieldName ];

        if ( formField.type === ASSET_FORM_FIELD_TYPE ) {

            const options = formField.options;

            const select = [];
            for ( let i in options ) {
                const assetName = options [ i ];
                const ingredient = this.ingredients [ assetName ];
                const disabled = ( ingredient.quantity - ingredient.utilizer ) > 0;
                select.push ({ key: i, text: assetName, value: assetName, disabled: disabled });
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
                        value = { this.state [ fieldName ] || '' }
                        onChange = { onChange }
                    />
                </div>
            );
        }
        
        //let inputType = field.fieldType === 'INTEGER' ? 'number' : 'string';

        return (
            <Form.Input
                fluid
                key = { fieldName }
                placeholder = { fieldName }
                name = { fieldName }
                //type = { inputType }
                type = { 'string' }
                value = { value }
                onChange = { onChange }
            />
        );
    }

    //----------------------------------------------------------------//
    render () {

        const formFields = this.formFields;
        const ingredients = this.ingredients;

        // add the fields in order
        let fields = [];
        for ( let fieldName in formFields ) {
            let formInput = this.makeFormInputForField ( fieldName, formFields [ fieldName ]);
            if ( formInput ) {
                fields.push ( formInput );
            }
        }

        const isSubmitEnabled = this.checkFormInputs ();

        let onClickSend = () => {
            this.props.onSubmit ( Object.assign ({}, this.state ))
        };

        let ingredientListItems = [];
        for ( let assetName in ingredients ) {
            let ingredient = ingredients [ assetName ];
            let available = ingredient.quantity - ingredient.utilized;
            if  ( available > 0 ) {
                ingredientListItems.push (<List.Item key = { assetName }>{ assetName + ' x' + ( ingredient.quantity - ingredient.utilized )}</List.Item>);
            }
        }

        return (
            <Form size = "large">
                <Segment stacked>
                    <List>{ ingredientListItems }</List>
                    { fields }
                    <Button type = 'button' color = "teal" fluid disabled = { !isSubmitEnabled } onClick = { onClickSend }>
                        OK
                    </Button>
                </Segment>
            </Form>
        );
    }
}

export default withAppState ( MethodForm );
