/* eslint-disable no-whitespace-before-property */

import { AssetView }                        from '../AssetView';
import { InventoryService }                 from '../InventoryService';
import { InventoryView }                    from '../InventoryView';
import { Service, useService }              from '../Service';
import { SingleColumnContainerView }        from '../SingleColumnContainerView'
import * as util                            from '../util/util';
import { action, computed, extendObservable, runInAction, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useState }                  from 'react';
import { Button, Card, Divider, Dropdown, Form, Grid, Header, Icon, Message, Modal, Segment, TextArea } from 'semantic-ui-react';

//================================================================//
// DebugSchemaScreenController
//================================================================//
class DebugSchemaScreenController extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        this.schema     = false;

        extendObservable ( this, {
            schemaJSON:     '',
            errorMsg:       false,
        });
    }

    //----------------------------------------------------------------//
    @computed get
    readyToSubmit () {

        return (( this.schemaJSON.length > 0 ) && ( this.schema !== false ));
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
// DebugSchemaScreen
//================================================================//
export const DebugSchemaScreen = observer (( props ) => {

    const inventory             = useService (() => new InventoryService ());
    const controller            = useService (() => new DebugSchemaScreenController ());

    let isSubmitEnabled = controller.readyToSubmit;

    const onRefresh = () => {
        inventory.reset ( controller.schema );
    }

    let assetLayouts = [];
    if ( inventory.loading === false ) {
        const assetArray = inventory.availableAssetsArray;
        for ( let i in assetArray ) {
            const asset = assetArray [ i ];
            assetLayouts.push (
                <Card
                    key = { asset.assetID }
                    style = {{ float:'left', border:`2px solid white` }}
                >
                    <AssetView
                        assetId = { asset.assetID }
                        inventory = { inventory }
                        inches = 'true'
                    />
                </Card>
            );
        }
    }

    return (
        <div>
            <SingleColumnContainerView>
                <Segment>
                    <Form error = { controller.errorMsg !== false }>
                    
                        <TextArea
                            placeholder = 'Schema JSON'
                            rows = { 8 }
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

                        <Button type = 'button' color = "red" fluid disabled = { !isSubmitEnabled } onClick = { onRefresh }>
                            Refresh
                        </Button>
                    </Form>
                </Segment>

            </SingleColumnContainerView>

            <If condition = { assetLayouts.length > 0 }>
                { assetLayouts }
            </If>
        </div>
    );
});
