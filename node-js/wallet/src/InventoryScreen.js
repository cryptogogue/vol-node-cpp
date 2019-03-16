/* eslint-disable no-whitespace-before-property */

import { withAppStateAndUser }          from './AppStateProvider';
import BaseComponent                    from './BaseComponent';
import MethodFormSelector               from './MethodFormSelector';
import NavigationBar                    from './NavigationBar';
import React                            from 'react';
import { Inventory, buildSchema }       from 'volition-schema-builder';
import { Segment, Grid } from 'semantic-ui-react';

const op = buildSchema.op;

const TEST = true;

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

        if ( TEST ) {

            let schemaTemplate = buildSchema ( 'TEST_SCHEMA', 'schema.lua' )

                //----------------------------------------------------------------//
                .assetTemplate ( 'base' )
                    .field ( 'displayName' ).string ()
            
                .assetTemplate ( 'card' ).extends ( 'base' )
                    .field ( 'keywords' ).string ().array ()

                //----------------------------------------------------------------//
                .assetDefinition ( 'pack', 'base' )
                    .field ( 'displayName', 'Booster Pack' )
            
                .assetDefinition ( 'common', 'card' )
                    .field ( 'displayName', 'Common' )
                    .field ( 'keywords', [ 'card', 'common' ])
            
                .assetDefinition ( 'rare', 'card' )
                    .field ( 'displayName', 'Rare' )
                    .field ( 'keywords', [ 'card', 'rare' ])
            
                .assetDefinition ( 'ulraRare', 'card' )
                    .field ( 'displayName', 'Ultra-Rare' )
                    .field ( 'keywords', [ 'card', 'ultra-rare' ])

                //----------------------------------------------------------------//
                .method ( 'makeRare', 1, 2, 'Combine two commons to make a rare.' )
                    .assetArg ( 'common0', op.ASSET_TYPE ( 'common' ))
                    .assetArg ( 'common1', op.ASSET_TYPE ( 'common' ))

                .method ( 'makeUltraRare', 1, 2, 'Combine two rares to make an ultra-rare.' )
                    .assetArg ( 'rare0', op.ASSET_TYPE ( 'rare' ))
                    .assetArg ( 'rare1', op.ASSET_TYPE ( 'rare' ))

                .method ( 'openPack', 1, 2, 'Open a booster pack.' )
                    .assetArg ( 'pack', op.ASSET_TYPE ( 'pack' ))

                .done ()

            let assets = [
                { className: 'pack', quantity: 1 },
                { className: 'common', quantity: 2 },
                { className: 'rare', quantity: 2 },
                { className: 'ultraRare', quantity: 1 },
            ];

            this.inventory = new Inventory ( schemaTemplate, assets );
            this.state.assets = assets;
            this.state.hasInventory = true;
        }
        else {
            this.fetchInventory ();
        }
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

        const inventory = this.inventory;

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
