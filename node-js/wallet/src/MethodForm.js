/* eslint-disable no-whitespace-before-property */

import { withAppState }             from './AppStateProvider';
import React, { Component }         from 'react';
import { Button, Form, Segment } from 'semantic-ui-react';

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

        const { formFields } = this.props;

        let fieldValues = {};
        for ( let fieldName in formFields ) {
            fieldValues [ fieldName ] = null;
        }

        this.state = fieldValues;
    }

    //----------------------------------------------------------------//
    handleChange ( event ) {
        
        let typedValue = null;
        const value = event.target.value;

        if ( value && ( value.length > 0 )) {
            const type = event.target.type;
            typedValue = ( type === 'number' ) ? Number ( value ) : String ( value );
        }

        this.setState ({[ event.target.name ]: typedValue });
    }

    //----------------------------------------------------------------//
    makeFormInputForField ( fieldName, formField ) {

        let onChange = ( event ) => { this.handleChange ( event )};

        let value = this.state [ fieldName ] === null ? '' : this.state [ fieldName ];

        if ( formField.type === 'asset' ) {

            const options = formField.options;

            // if ( accountKeyNames.length <= 1 ) return;

            return (
                <div key = { fieldName }>
                    <Form.Input
                        fluid
                        list = 'listOfAssetOptions'
                        placeholder = 'Asset'
                        name = { fieldName }
                        value = { options [ 0 ]}
                        onChange = { onChange }
                    />
                    <datalist id = 'listOfAssetOptions'>
                        { options.map (( assetName, index ) =>
                            <option key = { assetName } value = { assetName }/>
                        )}
                    </datalist>
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

        const { formFields } = this.props;

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

        return (
            <Form size = "large">
                <Segment stacked>
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
