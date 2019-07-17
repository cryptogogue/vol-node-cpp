/* eslint-disable no-whitespace-before-property */

import { AssetFormatter }                                   from '../util/AssetFormatter';
import { barcodeToSVG }                                     from '../util/pdf417';
import { meta }                                             from '../resources/meta';
import { Service }                                          from './Service';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { Binding, Schema, buildSchema }                     from 'volition-schema-builder';

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

        const op = buildSchema.op;

        let schemaTemplate = buildSchema ( 'TEST_SCHEMA', 'schema.lua' )
            .meta ( meta )

            //----------------------------------------------------------------//
            .definition ( 'pack' )
         
            .definition ( 'common' )
                .field ( 'keywords', 'card common' )
         
            .definition ( 'rare' )
                .field ( 'keywords', 'card rare' )
         
            .definition ( 'ultraRare' )
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

            // compose the asset
            let asset = this.formatter.composeAsset ( this.assets [ assetID ], [ 'EN', 'RGB' ]);

            // generate the barcode and inject it into the layout
            let barcode = barcodeToSVG ( assetID );
            this.assetLayouts [ assetID ] = this.formatter.formatAssetLayout ( asset, { barcode: barcode });

            // store the composed asset
            this.assets [ assetID ] = asset;
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
