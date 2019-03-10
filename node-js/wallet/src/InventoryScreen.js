/* eslint-disable no-whitespace-before-property */

import { withAppStateAndUser }  from './AppStateProvider';
import BaseComponent            from './BaseComponent';
import MethodFormSelector       from './MethodFormSelector';
import NavigationBar            from './NavigationBar';
import React                    from 'react';
import { Inventory }            from 'volition-schema-builder';

import { Dropdown, Segment, Header, Icon, Divider, Modal, Grid } from 'semantic-ui-react';

//================================================================//
// InventoryScreen
//================================================================//
class InventoryScreen extends BaseComponent {
    
    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            assets : [],
            hasInventory: false,
        };
        this.fetchInventory ();
    }

    //----------------------------------------------------------------//
    async fetchInventory () {

        const accountId = this.getAccountId ();
        const { minerURLs } = this.props.appState;

        if ( minerURLs.size === 0 ) {
            this.revocableTimeout (() => { this.fetchInventory ()}, 100 );
            return;
        }

        try {

            // ugh
            let url;
            minerURLs.forEach (( minerURL ) => {
                url = minerURL;
            });

            const schemaJSON = await this.revocableFetchJSON ( url + '/schemas/' );

            if ( schemaJSON.schemas ) {

                // just grab the first schema for now
                const schemaTemplate = schemaJSON.schemas [ 0 ];

                const inventoryJSON = await this.revocableFetchJSON ( url + '/accounts/' + accountId + '/inventory' );

                console.log ( inventoryJSON );

                if ( inventoryJSON.inventory ) {
                    let assets = inventoryJSON.inventory.assets;
                    this.inventory = new Inventory ( schemaTemplate, assets );
                    this.setState ({ assets : assets, hasInventory : true });
                }
            }
        }
        catch ( error ) {
            console.log ( error );
        }
    }

    //----------------------------------------------------------------//
    getAccountId () {
        let accountId = this.props.match.params && this.props.match.params.accountId;
        return accountId && ( accountId.length > 0 ) && accountId;
    }

    //----------------------------------------------------------------//
    render () {

        const { assets } = this.state;
        const inventory = this.inventory;

        // return (
        //     <div>
        //         { assets.map (( asset, i ) => {
        //             return (
        //                 <div key = { 'asset' + i }>
        //                 <div>{ asset.className } x { asset.quantity }</div>
        //                 { asset.methodNames.map (( methodName, j ) => {
        //                     return <div key = { asset.className + '.method.' + j }>{ '....' + methodName }</div>
        //                 })}
        //                 </div>
        //             );
        //         })}
        //     </div>
        // );

        return (
            <div>
                <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                    <Grid.Column style = {{ maxWidth: 450 }}>

                        <NavigationBar navTitle = "Inventory" match = { this.props.match }/>

                        <Segment>
                            <MethodFormSelector inventory = { inventory }/>
                        </Segment>

                    </Grid.Column>
                </Grid>
            </div>
        );
    }
}

export default withAppStateAndUser ( InventoryScreen );
