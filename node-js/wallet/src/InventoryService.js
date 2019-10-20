/* eslint-disable no-whitespace-before-property */

import { AssetLayout }                          from './AssetLayout';
import { barcodeToSVG }                         from './util/pdf417';
import { Service }                              from './Service';
import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { Binding }                              from './schema/Binding';
import { Schema }                               from './schema/Schema';
import { buildSchema, op }                      from './schema/SchemaBuilder';
import { JUSTIFY }                              from './util/TextFitter';
import handlebars                               from 'handlebars';
import _                                        from 'lodash';
import * as opentype                            from 'opentype.js';

//================================================================//
// InventoryService
//================================================================//
export class InventoryService extends Service {

    @observable loading         = true;

    @observable assets          = {};
    @observable assetLayouts    = {};
    @observable schema          = new Schema (); // empty schema
    @observable binding         = new Binding (); // empty binding

    //----------------------------------------------------------------//
    composeAssetContext ( asset, filters, overrideContext ) {

        let context = {};

        for ( let fieldName in asset.fields ) {

            const field = asset.fields [ fieldName ];
            const alternates = field.alternates;

            context [ fieldName ] = field.value;

            for ( let i in filters ) {
                const filter = filters [ i ];
                if ( _.has ( alternates, filter )) {
                    context [ fieldName ] = alternates [ filter ];
                }
            }
        }
        return Object.assign ( context, overrideContext );
    }

    //----------------------------------------------------------------//
    constructor ( appState, onProgress ) {
        super ();

        extendObservable ( this, {
            appState:   appState,
        });

        this.onProgress = onProgress;
        this.templates = {};
        this.layouts = {};
        this.fonts = {};

        this.fetchInventory ( appState.accountID, appState.node );

        observe ( appState, 'assetsUtilized', ( change ) => {
            console.log ( 'ASSETS UTILIZED DID CHANGE' );
            this.refreshBinding ();
        });
    }

    //----------------------------------------------------------------//
    async fetchInventory ( accountID, minerURL ) {

        try {
            console.log ( 'FETCH INVENTORY', accountID, minerURL );

            this.onProgress ( 'Fetching Schema' );
            const schemaJSON        = await this.revocableFetchJSON ( minerURL + '/schemas', null, 20000 );
            console.log ( schemaJSON );

            this.onProgress ( 'Fetching Inventory' );
            const inventoryJSON     = await this.revocableFetchJSON ( minerURL + '/accounts/' + accountID + '/inventory', null, 20000 );
            console.log ( inventoryJSON );

            let assets = {};
            for ( let asset of inventoryJSON.inventory ) {
                assets [ asset.assetID ] = asset;
            }
            await this.update ( schemaJSON.schemas, assets );
        }
        catch ( error ) {
            console.log ( error );
        }
        await this.finishLoading ();
    }

    //----------------------------------------------------------------//
    @action
    finishLoading () {
        this.loading = false;
    }

    //----------------------------------------------------------------//
    @action
    getAssetLayout ( assetID ) {
        if ( !_.has ( this.assetLayouts, assetID )) {
            this.assetLayouts [ assetID ] = new AssetLayout ( this, assetID, [ 'EN', 'RGB' ]);
        }
        return this.assetLayouts [ assetID ];
    }

    //----------------------------------------------------------------//
    @computed
    get availableAssetsArray () {

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
    @computed
    get availableAssetsByID () {

        const assetsUtilized = this.appState.assetsUtilized;

        let assets = {};
        for ( let assetID in this.assets ) {
            if ( !assetsUtilized.includes ( assetID )) {
                assets [ assetID ] = this.assets [ assetID ];
            }
        }
        return assets;
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
    refreshAssetLayouts () {
        this.assetLayouts = {};
    }

    //----------------------------------------------------------------//
    @action
    refreshBinding ( schema, assets ) {

        this.schema = schema || this.schema;
        this.assets = assets || this.assets;

        const availableAssetsByID = this.availableAssetsByID;
        console.log ( 'REFRESH BINDING:', availableAssetsByID );
        this.binding = this.schema.generateBinding ( availableAssetsByID );
    }

    //----------------------------------------------------------------//
    async update ( templates, assets ) {

        let schema = new Schema ();

        for ( let template of templates ) {
            this.onProgress ( 'Applying Template' );
            await schema.applyTemplate ( template );

            this.onProgress ( 'Compiling Layouts' );
            for ( let layoutName in template.layouts ) {
                const layout = _.cloneDeep ( template.layouts [ layoutName ]);
                for ( let command of layout.commands ) {
                    command.template = handlebars.compile ( command.template );
                }
                this.layouts [ layoutName ] = layout;
            }

            this.onProgress ( 'Fetching Fonts' );
            for ( let name in template.fonts ) {

                try {
                    const url = template.fonts [ name ].url;

                    console.log ( 'FETCHING FONT:', name, url );

                    const response  = await this.revocableFetch ( url );
                    const buffer    = await response.arrayBuffer ();
                    this.fonts [ name ] = opentype.parse ( buffer );
                }
                catch ( error ) {
                    console.log ( error );
                }
            }
        }

        // TODO: properly handle layout field alternatives; doing this here is a big, fat hack
        let assetsWithLayouts = {};
        assets = assets || {};
        for ( let assetID in assets ) {

            const asset = assets [ assetID ];
            const layoutName = _.has ( asset.fields, 'layout' ) ? asset.fields.layout.value : '';
            
            if ( this.layouts [ layoutName ]) {
                assetsWithLayouts [ assetID ] = asset;
            }
        }

        this.onProgress ( 'Refreshing Binding' );
        this.refreshBinding ( schema, assetsWithLayouts );
        this.refreshAssetLayouts ();
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
