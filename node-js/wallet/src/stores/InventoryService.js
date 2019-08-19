/* eslint-disable no-whitespace-before-property */

import { AssetFormatter }                                   from '../util/AssetFormatter';
import { barcodeToSVG }                                     from '../util/pdf417';
import { meta }                                             from '../resources/meta';
import { Service }                                          from './Service';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { Binding }                                          from '../schema/Binding';
import { Schema }                                           from '../schema/Schema';
import { buildSchema, op }                                  from '../schema/SchemaBuilder';

const DEBUG = true;

//================================================================//
// InventoryService
//================================================================//
export class InventoryService extends Service {

    @observable loading     = true;

    @observable assets          = {};
    @observable assetLayouts    = {};
    @observable schema          = new Schema (); // empty schema
    @observable binding         = new Binding (); // empty binding

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    // computed

    //----------------------------------------------------------------//
    @computed get
    availableAssetsArray () {

        const assetsUtilized = this.appState.assetsUtilized;

        let assets = [];
        for ( let assetID in this.assets ) {
            if ( !assetsUtilized.includes ( assetID )) {
                assets.push ( this.assets [ assetID ]);
            }
        }
        return assets;
    }

    //----------------------------------------------------------------//
    @computed get
    availableAssetsByID () {

        const assetsUtilized = this.appState.assetsUtilized;

        let assets = {};
        for ( let assetID in this.assets ) {
            if ( !assetsUtilized.includes ( assetID )) {
                assets [ assetID ] = this.assets [ assetID ];
            }
        }
        return assets;
    }

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    // methods

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        extendObservable ( this, {
            appState:   appState,
        });

        this.formatter = new AssetFormatter ();

        if ( DEBUG || ( !( appState.accountId && appState.node ))) {
            this.useDebugInventory ();
        }
        else {
            this.fetchInventory ( appState.accountId, appState.node );
        }

        observe ( appState, 'assetsUtilized', ( change ) => {
            console.log ( 'ASSETS UTILIZED DID CHANGE' );
            this.refreshBinding ();
        });
    }

    //----------------------------------------------------------------//
    async fetchInventory ( accountID, minerURL ) {

        try {
            const schemaJSON = await this.revocableFetchJSON ( minerURL + '/schemas/' );
            const inventoryJSON = await this.revocableFetchJSON ( minerURL + '/accounts/' + accountID + '/inventory' );

            this.update ( schemaJSON.schemas, inventoryJSON.inventory );
            this.finishLoading ();
        }
        catch ( error ) {
            console.log ( error );
            this.finishLoading ();
        }
    }

    //----------------------------------------------------------------//
    @action
    finishLoading () {
        this.loading = false;
    }

    //----------------------------------------------------------------//
    getCraftingMethodBindings () {
        return this.binding.methodBindingsByName;
    }

    //----------------------------------------------------------------//
    getCraftingMethodBindingsForAssetID ( assetID ) {
        return this.binding.methodBindingsByAssetID [ assetID ];
    }

    //----------------------------------------------------------------//
    methodIsValid ( methodName, assetID ) {
        return ( methodName !== '' ) && this.binding.methodIsValid ( methodName, assetID );
    }

    //----------------------------------------------------------------//
    @action
    refreshBinding () {
        const availableAssetsByID = this.availableAssetsByID;
        console.log ( 'REFRESH BINDING:', availableAssetsByID );
        this.binding = this.schema.generateBinding ( availableAssetsByID );
    }

    //----------------------------------------------------------------//
    @action
    update ( templates, assets ) {

        if ( templates ) {
            this.schema = new Schema ();
            for ( let i in templates ) {
                this.schema.applyTemplate ( templates [ i ]);
                this.formatter.applyMeta ( templates [ i ].meta );
            }
        }

        if ( assets ) {
            this.assets = assets;
        }
        this.refreshBinding ();
    }

    //----------------------------------------------------------------//
    @action
    useDebugInventory () {

        let schemaTemplate = buildSchema ( 'TEST_SCHEMA', 'schema.lua' )
            .meta ( meta )

            //----------------------------------------------------------------//
            .definition ( 'pack' )
                .field ( 'layout', 'pack' )
                .field ( 'displayName', 'Pack' )
                    .alternate ( 'ES', 'El Pack' )
                    .alternate ( 'FR', 'Le Pack' )
         
            .definition ( 'common' )
                .field ( 'layout', 'card' )
                .field ( 'displayName', 'Common' )
                    .alternate ( 'ES', 'El Common' )
                    .alternate ( 'FR', 'Le Common' )
                .field ( 'image', 'https://i.imgur.com/VMPKVAN.jpg' )
                .field ( 'keywords', 'card common' )
         
            .definition ( 'rare' )
                .field ( 'layout', 'card' )
                .field ( 'displayName', 'Rare' )
                    .alternate ( 'ES', 'El Rare' )
                    .alternate ( 'FR', 'Le Rare' )
                .field ( 'image', 'https://i.imgur.com/BtKggd4.jpg' )
                .field ( 'keywords', 'card rare' )
         
            .definition ( 'ultraRare' )
                .field ( 'layout', 'card' )
                .field ( 'displayName', 'Ultra-Rare' )
                    .alternate ( 'ES', 'El Ultra-Rare' )
                    .alternate ( 'FR', 'Le Ultra-Rare' )
                .field ( 'image', 'https://i.imgur.com/2aiJ3cq.jpg' )
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

        let schema = new Schema ( schemaTemplate );

        let assets = {};

        const pack0         = schema.addTestAsset ( assets, 'pack', 'bofip-jitut-vupoz-208' );
        const common0       = schema.addTestAsset ( assets, 'common', 'dosaz-huvuf-nohol-103' );
        const common1       = schema.addTestAsset ( assets, 'common', 'famaz-havij-zohag-209' );
        const rare0         = schema.addTestAsset ( assets, 'rare', 'giduv-zotav-domin-184' );
        const rare1         = schema.addTestAsset ( assets, 'rare', 'honas-simuj-marif-114' );
        const ultraRare0    = schema.addTestAsset ( assets, 'ultraRare', 'jafoh-najon-gobig-250' );

        this.schema = schema;
        this.assets = assets;
        this.refreshBinding ();

        this.formatter.applyMeta ( schemaTemplate.meta );

        for ( let assetID in this.assets ) {

            const asset = this.assets [ assetID ];
            const filters = [ 'EN', 'RGB' ];

            // generate the barcode and inject it into the layout
            let barcode = barcodeToSVG ( assetID );
            this.assetLayouts [ assetID ] = this.formatter.composeAssetLayout ( asset, filters, { barcode: barcode });
        }

        this.finishLoading ();
    }

    //----------------------------------------------------------------//
    @computed get
    validMethods () {

        let methods = [];
        const bindingsByName = this.binding.methodBindingsByName;
        for ( let name in bindingsByName ) {
            if ( bindingsByName [ name ].valid ) {
                methods.push ( name );
            }
        }
        return methods;
    }
}
