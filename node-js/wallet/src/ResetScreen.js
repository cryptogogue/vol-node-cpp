/* eslint-disable no-whitespace-before-property */

import { AccountInfoService }               from './AccountInfoService';
import { AppStateService }                  from './AppStateService';
import { NavigationBar }                    from './NavigationBar';
import { NodeInfoService }                  from './NodeInfoService';
import { Service, useService }              from './Service';
import { SingleColumnContainerView }        from './SingleColumnContainerView'
import { Transaction, TRANSACTION_TYPE }    from './Transaction';
import * as util                            from './util/util';
import { action, computed, extendObservable, runInAction, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Divider, Dropdown, Form, Grid, Header, Icon, Message, Modal, Segment, TextArea } from 'semantic-ui-react';

//================================================================//
// ResetScreenController
//================================================================//
class ResetScreenController extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        this.appState   = appState;
        this.schema     = false;

        extendObservable ( this, {
            nodeURL:        appState.node,
            password:       '',
            schemaJSON:     '',
            busy:           false,
            errorMsg:       false,
        });
    }

    //----------------------------------------------------------------//
    @action
    handleSubmit () {
    
        const appState = this.appState;
        const password = this.password;

        this.busy = true;

        const _fetch = async () => {
            try {

                await this.revocableFetch ( this.nodeURL + '/', {
                    method :    'DELETE'
                });

                appState.clearStagedTransactions ();
                appState.clearPendingTransactions ();

                if ( this.schema ) {

                    await AccountInfoService.update ( this, appState );
                    await NodeInfoService.update ( this, appState, this.nodeURL );

                    const maker = {
                        gratuity:       0,
                        accountName:    appState.accountID,
                        keyName:        appState.keyName,
                        nonce:          -1,
                    };

                    const publishSchemaBody = {
                        maker:          maker,
                        schema:         this.schema,
                    };
                    appState.pushTransaction ( Transaction.transactionWithBody ( TRANSACTION_TYPE.PUBLISH_SCHEMA, publishSchemaBody ));

                    const betaGetAssetsBody = {
                        maker:          maker,
                        numAssets:      1,
                    };
                    appState.pushTransaction ( Transaction.transactionWithBody ( TRANSACTION_TYPE.BETA_GET_ASSETS, betaGetAssetsBody ));

                    await appState.submitTransactions ( password );
                    this.setPassword ();
                    this.setSchemaJSON ();
                }

                runInAction (() => {
                    this.busy = false;
                });
            }
            catch ( error ) {
                console.log ( error );
            }
        }
        _fetch ();
    }

    //----------------------------------------------------------------//
    @computed get
    readyToSubmit () {

        return (
            ( this.busy === false ) &&
            ( this.nodeURL.length > 0 ) &&
            (( this.schemaJSON.length > 0 ) ? (( this.schema !== false ) && this.appState.checkPassword ( this.password )) : true )
        );
    }

    //----------------------------------------------------------------//
    @action
    setNodeURL ( nodeURL ) {
        this.nodeURL = nodeURL || '';
    }

    //----------------------------------------------------------------//
    @action
    setPassword ( password ) {
        this.password = password || '';
    }

    //----------------------------------------------------------------//
    @action
    setSchemaJSON ( schemaJSON ) {

        this.schemaJSON = schemaJSON || '';
        try {
            this.schema = this.schemaJSON.length > 0 ? JSON.parse ( this.schemaJSON ) : false;
            this.errorMsg = false;
        }
        catch ( error ) {
            this.schema = false;
            this.errorMsg = 'Invalid schema JSON.';
        }
    }
}

//================================================================//
// ResetScreen
//================================================================//
export const ResetScreen = observer (( props ) => {

    const accountIDFromEndpoint = util.getMatch ( props, 'accountID' );

    const appState              = useService (() => new AppStateService ( util.getMatch ( props, 'userID' ), accountIDFromEndpoint ));
    const controller            = useService (() => new ResetScreenController ( appState ));

    const accountID             = appState.accountID;

    // TODO: move redirects to a HOC
    if ( !appState.hasUser ()) return appState.redirect ( '/' );
    if ( !appState.isLoggedIn ()) return appState.redirect ( '/login' );

    if ( accountID !== accountIDFromEndpoint ) {
        return appState.redirect ( `/accounts/${ accountID }/reset` );
    }

    // if ( controller.showInventory ) return appState.redirect ( `/accounts/${ accountID }/inventory` );

    let isSubmitEnabled = controller.readyToSubmit;

    return (
        <SingleColumnContainerView>

            <NavigationBar navTitle = 'Reset' appState = { appState }/>

            <Segment>
                <Form error = { controller.errorMsg !== false }>

                    <Form.Input
                        fluid
                        icon = 'lock'
                        iconPosition = 'left'
                        placeholder = 'Wallet Password'
                        type = 'password'
                        autoComplete = 'password'
                        value = { controller.password }
                        onChange = {( event ) => { controller.setPassword ( event.target.value )}}
                        disabled = { controller.busy }
                    />

                    <Form.Input
                        fluid
                        placeholder = 'Node URL'
                        name = 'nodeURL'
                        value = { controller.nodeURL }
                        onChange = {( event ) => { controller.setNodeURL ( event.target.value )}}
                        disabled = { controller.busy }
                    />
                
                    <TextArea
                        placeholder = 'Schema JSON'
                        rows = { 16 }
                        value = { controller.schemaJSON }
                        onChange = {( event ) => { controller.setSchemaJSON ( event.target.value )}}
                        disabled = { controller.busy }
                    />
                    
                    <Message
                        error
                        header  = 'Error'
                        content = { controller.errorMsg }
                    />

                    <div className = "ui hidden divider" ></div>

                    <Button type = 'button' color = "red" fluid disabled = { !isSubmitEnabled } onClick = {() => { controller.handleSubmit ()}}>
                        Reset Mining Node
                    </Button>
                </Form>
            </Segment>

        </SingleColumnContainerView>
    );
});
