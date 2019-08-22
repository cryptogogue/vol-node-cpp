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

import { TEST_SCHEMA }                          from './resources/sample-schema';

const DEBUG = true;

//================================================================//
// InventoryService
//================================================================//
export class InventoryService extends Service {

    @observable loading         = true;

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
    constructor ( appState ) {
        super ();

        extendObservable ( this, {
            appState:   appState,
        });

        this.templates = {};
        this.layouts = {};
        this.fonts = {};

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
    refreshAssetLayouts () {

        this.assetLayouts = {};
        for ( let assetId in this.assets ) {
            this.assetLayouts [ assetId ] = new AssetLayout ( this, assetId, [ 'EN', 'RGB' ]);
        }
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
    async useDebugInventory () {

        let template = TEST_SCHEMA;
        let schema = new Schema ();
        await schema.applyTemplate ( template );

        for ( let layoutName in template.layouts ) {
            const layout = template.layouts [ layoutName ];
            for ( let command of layout.commands ) {
                command.template = handlebars.compile ( command.template );
            }
            this.layouts [ layoutName ] = layout;
        }

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

        let assets = {};
        for ( let assetType in schema.definitions ) {

            const definition = schema.definitions [ assetType ];
            const layoutName = _.has ( definition.fields, 'layout' ) ? definition.fields.layout.value : '';
            
            if ( this.layouts [ layoutName ]) {
                schema.addTestAsset ( assets, assetType );
            }
        }

        this.refreshBinding ( schema, assets );
        this.refreshAssetLayouts ();
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
