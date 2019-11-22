/* eslint-disable no-whitespace-before-property */

import { AssetView }                        from '../AssetView';
import { InventoryService }                 from '../InventoryService';
import { InventoryView }                    from '../InventoryView';
import { schemaFromXLSX }                   from '../schema/schemaFromXLSX';
import { Service, useService }              from '../Service';
import { SingleColumnContainerView }        from '../SingleColumnContainerView';
import * as excel                           from '../util/excel';
import * as util                            from '../util/util';
import { action, computed, extendObservable, runInAction, observable, observe } from 'mobx';
import { observer }                         from 'mobx-react';
import React, { useRef, useState }          from 'react';
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
    setSchema ( schema ) {

        this.schema = schema;
        this.schemaJSON = JSON.stringify ( schema, null, 4 );
        this.errorMsg = false;
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

    const inventory                         = useService (() => new InventoryService ());
    const controller                        = useService (() => new DebugSchemaScreenController ());

    const [ file, setFile ]                 = useState ( false );
    const [ fileString, setFileString ]     = useState ( '' );
    const filePickerRef                     = useRef ();

    const reloadFile = ( picked ) => {

        const reader = new FileReader ();

        reader.onabort = () => { console.log ( 'file reading was aborted' )}
        reader.onerror = () => { console.log ( 'file reading has failed' )}
        reader.onload = () => {
            const book = new excel.Workbook ( reader.result, { type: 'binary' });
            if ( book ) {
                const schemaJSON = schemaFromXLSX ( book );
                console.log ( schemaJSON );
                controller.setSchema ( schemaJSON );
                inventory.reset ( controller.schema );
            }
        }
        reader.readAsBinaryString ( picked );
    }

    const onFilePickerChange = ( event ) => {
        const picked = event.target.files.length > 0 ? event.target.files [ 0 ] : false;
        if ( picked ) {
            setFile ( picked );
            reloadFile ( picked );
        }
    }

    const hasFile = ( file !== false );

    return (
        <div style = {{
            display: 'flex',
            flexFlow: 'column',
            height: '100vh',
        }}>
            <SingleColumnContainerView title = 'Test Schema'>

                <input
                    style = {{ display:'none' }}
                    ref = { filePickerRef }
                    type = 'file'
                    accept = ".xls, .xlsx"
                    onChange = { onFilePickerChange }
                />
            
                <Segment>
                    <Form size = 'large' error = { false }>
                    
                        <Button
                            fluid
                            onClick = {() => filePickerRef.current.click ()}
                        >
                            Choose File
                        </Button>

                        <div className = 'ui hidden divider' ></div>

                        <Button
                            fluid
                            disabled = { !hasFile }
                            onClick = {() => { reloadFile ( file )}}
                        >
                            { hasFile ? file.name : 'No File Chosen' }
                        </Button>

                        <Message
                            error
                            header  = 'Error'
                            content = { controller.errorMsg }
                        />
                    </Form>
                </Segment>

            </SingleColumnContainerView>

            <If condition = { inventory.loading === false }>
                <div style = {{ flex: 1 }}>
                    <InventoryView
                        inventory = { inventory }
                    />
                </div>
            </If>
        </div>
    );
});
