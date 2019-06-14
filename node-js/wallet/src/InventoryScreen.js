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

            let schemaTemplate = buildSchema ( 'TEST_SCHEMA' )

                //----------------------------------------------------------------//
                .definition ( 'pack' )
                    .field ( 'displayName', 'Booster Pack' )
             
                .definition ( 'common' )
                    .field ( 'displayName', 'Common' )
                    .field ( 'keywords', 'card common' )
             
                .definition ( 'rare' )
                    .field ( 'displayName', 'Rare' )
                    .field ( 'keywords', 'card rare' )
             
                .definition ( 'ultraRare' )
                    .field ( 'displayName', 'Ultra-Rare' )
                    .field ( 'keywords', 'card ultra-rare' )

                //----------------------------------------------------------------//
                .method ( 'makeRare', 'Combine two commons to make a rare.' )
                    .assetArg ( 'common0', op.ASSET_TYPE ( 'common' ))
                    .assetArg ( 'common1', op.ASSET_TYPE ( 'common' ))

                .method ( 'makeUltraRare', 'Combine two rares to make an ultra-rare.' )
                    .assetArg ( 'rare0', op.ASSET_TYPE ( 'rare' ))
                    .assetArg ( 'rare1', op.ASSET_TYPE ( 'rare' ))

                .method ( 'openPack', 'Open a booster pack.' )
                    .assetArg ( 'pack', op.ASSET_TYPE ( 'pack' ))

                .done ()

            this.inventory = new Inventory ( schemaTemplate );

            this.inventory.addTestAssets ( 'pack', 1 );
            this.inventory.addTestAssets ( 'common', 2 );
            this.inventory.addTestAssets ( 'rare', 2 );
            this.inventory.addTestAssets ( 'ultraRare', 1 );

            this.inventory.process ();

            //this.state.assets = assets;
            //this.state.hasInventory = true;
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
