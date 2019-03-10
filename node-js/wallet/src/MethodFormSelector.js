/* eslint-disable no-whitespace-before-property */

import { withAppState }                 from './AppStateProvider';
import MethodForm                       from './MethodForm';
import { transactionSchemas }           from './TransactionSchemas';
import React, { Component }             from 'react';
import { Button, Divider, Dropdown }    from 'semantic-ui-react';

//================================================================//
// MethodFormSelector
//================================================================//
class MethodFormSelector extends Component {

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            methods: [],
            methodIndex: -1,
            showForm: false,
        };
    }

    //----------------------------------------------------------------//
    static getDerivedStateFromProps ( nextProps, prevState ) {

        const inventory = nextProps.inventory;
        if ( inventory ) {

            let methods = [];

            for ( let methodName in inventory.methodBindings ) {
                methods.push ( methodName );
            }
            return { methods: methods };
        }
        return null;
    }

    //----------------------------------------------------------------//
    handleSubmit ( fieldValues ) {

        // fieldValues.makerNonce = this.props.nonce;

        // this.showForm ( false );
        // this.props.appState.startTransaction ( schema, fieldValues );
    }

    //----------------------------------------------------------------//
    render () {

        const { methods }  = this.state;
        const { inventory } = this.props;

        // const pendingTransaction = appState.getPendingTransaction ( accountId );
        // if ( pendingTransaction ) {
        //     return this.renderPendingTransaction ( pendingTransaction );
        // }

        const hasMethods = ( methods.length > 0 );

        if ( hasMethods && this.state.showForm ) {

            let methodForm;

            const methodIndex = this.state.methodIndex;

            if ( 0 <= methodIndex ) {

                methodForm = (
                    <MethodForm
                        formFields = { inventory.getMethodFormFields ( methods [ methodIndex ])}
                        onSubmit = {( fieldValues ) => { this.handleSubmit ( fieldValues )}}
                    />
                );
            }

            return (
                <div>
                    { this.renderDropdown ()}
                    { methodForm }
                    <Divider/>
                    <Button color = "red" fluid onClick = {() => { this.showForm ( false )}}>
                        Cancel
                    </Button>
                </div>
            );
        }

        return (
            <div>
                <Button color = "teal" fluid disabled = { !hasMethods } onClick = {() => { this.showForm ( true )}}>
                    New Action
                </Button>
            </div>
        );
    }

    //----------------------------------------------------------------//
    renderDropdown () {

        const { methods } = this.state;
        let options = [];

        for ( let i in methods ) {
            options.push ({ key:methods [ i ], value:i, text:methods [ i ]});
        }

        return (
            <Dropdown
                placeholder = "Select Action"
                fluid
                search
                selection
                options = { options }
                onChange = {( event, data ) => { this.selectForm ( data.value )}}
            />
        );
    }

    //----------------------------------------------------------------//
    renderPendingTransaction ( transaction ) {

        // return (
        //     <p>{ transaction.friendlyName }</p>
        // );
    }

    //----------------------------------------------------------------//
    selectForm ( index ) {

        this.setState ({ methodIndex: index });
    }

    //----------------------------------------------------------------//
    showForm ( show ) {

        this.setState ({
            showForm: show,
            methodIndex: -1,
        });
    }
}

export default withAppState ( MethodFormSelector );
