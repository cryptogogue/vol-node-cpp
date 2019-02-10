/* eslint-disable no-whitespace-before-property */

import { withAppStateAndUser }  from './AppStateProvider';
import BaseComponent            from './BaseComponent';
import React                    from 'react';
import { Schema }               from './utils/schema';
// import { Dropdown, Segment, Header, Icon, Divider, Modal, Grid } from 'semantic-ui-react';

//================================================================//
// InventoryScreen
//================================================================//
class InventoryScreen extends BaseComponent {
    
    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            assets : [],
            schema: {},
        };

        this.schema = new Schema ();
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

                schemaJSON.schemas.forEach (( template ) => {
                    this.schema.applyTemplate ( template );
                });

                // just grab the first schema for now
                this.setState ({ schema : schemaJSON.schemas [ 0 ]});

                const inventoryJSON = await this.revocableFetchJSON ( url + '/accounts/' + accountId + '/inventory' );

                if ( inventoryJSON.inventory ) {

                    let inventory = inventoryJSON.inventory;
                    let assets = inventory.assets;
                    
                    const binding = this.schema.processInventory ( inventory );
                    console.log ( 'BINDING', binding );

                    for ( let i in assets ) {
                        let asset = assets [ i ];
                        asset.methodNames = [];
                        //asset.methodNames = this.schema.getMethodNamesForAsset ( asset, inventory );
                    }
                    this.setState ({ assets : assets });
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

        // let methodList;
        // if ( schema && schema.methods ) {
        //     methodList = Object.keys ( schema.methods ).map (( methodName ) => {
        //         return (<p key = { 'method.' + methodName }>method: { methodName }</p>);
        //     });
        // }

        return (
            <div>
                { assets.map (( asset, i ) => {
                    return (
                        <div key = { 'asset' + i }>
                        <div>{ asset.className } x { asset.quantity }</div>
                        { asset.methodNames.map (( methodName, j ) => {
                            return <div key = { asset.className + '.method.' + j }>{ '....' + methodName }</div>
                        })}
                        </div>
                    );
                })}
            </div>
        );
    }
}

export default withAppStateAndUser ( InventoryScreen );
