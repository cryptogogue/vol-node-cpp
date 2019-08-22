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

import { SAMPLE_SCHEMA }                        from './resources/sample-schema';

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

        let template = buildSchema ( 'TEST_SCHEMA', 'schema.lua' )

            //----------------------------------------------------------------//
            .font ( 'roboto', 'http://localhost:3000/fonts/roboto/roboto-regular.ttf' )

            //----------------------------------------------------------------//
            .layout ( 'dude', 750, 1050, 300 )
                .drawSVG (`
                    <rect x='0' y='0' width='750' height='1050' fill='#000000'/>
                    <rect x='37.5' y='37.5' width='675' height='975' fill='#ffffff'/>
                    <rect x='37.5' y='168.75' width='675' height='412.5' fill='#ff0000'/>
                    <rect x='37.5' y='900' width='675' height='112.5' fill='#ff0000'/>

                    <rect x='48.875' y='37.5' width='534.375' height='56.25' fill='none' stroke='gray'/>
                    <rect x='48.875' y='93.75' width='534.375' height='37.5' fill='none' stroke='gray'/>
                    <rect x='48.875' y='131.25' width='534.375' height='37.5' fill='none' stroke='gray'/>

                    <rect x='48.875' y='592.625' width='652.25' height='296' fill='none' stroke='gray'/>

                    <image x='37.5' y='168.75' width='675' height='412.5' xlink:href='{{ image }}'/>
                `)
                // card name
                .drawText ( '{{ name }}', 'roboto', 40, 48.875, 37.5, 534.375, 56.25 )
                    .justify ( JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.CENTER )

                // card type
                .drawText ( '{{ type }}{{ subType }}', 'roboto', 30, 48.875, 93.75, 534.375, 37.5 )
                    .justify ( JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.CENTER )

                // access
                .drawText ( '{{ access }}', 'roboto', 30, 48.875, 131.25, 534.375, 37.5)
                    .justify ( JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.CENTER )

                // rules
                .drawText ( '{{ rules }}', 'roboto', 40, 48.875, 592.625, 652.25, 296 )
                    .justify ( JUSTIFY.HORIZONTAL.LEFT, JUSTIFY.VERTICAL.TOP )

                .drawBarcode( '{{ $ }}', 37.5, 900, 675, 112.5 )

            .done ();

            // .layout ( 'pack', 750, 1050, 300 )
            //     .drawSVG (`
            //         <rect x='0' y='0' width='750' height='1050' fill='#000000'/>
            //         <rect x='37.5' y='37.5' width='675' height='975' fill='gray'/>
            //         <text x="375" y="560" font-size="150" text-anchor="middle" fill="white">PACK</text>
            //     `)
            //     .drawTextField ( 'displayName', 'roboto', 70, 0, 815, 700, 70 )
            //         .justify ( JUSTIFY.HORIZONTAL.CENTER, JUSTIFY.VERTICAL.CENTER )
            //         .pen ( 'white' )
            //     .drawBarcodeField ( '$', 37.5, 900, 675, 112.5 )

            // //----------------------------------------------------------------//
            // .definition ( 'pack' )
            //     .field ( 'layout', 'pack' )
            //     .field ( 'displayName', 'Pack' )
            //         .alternate ( 'ES', 'El Pack' )
            //         .alternate ( 'FR', 'Le Pack' )
         
            // .definition ( 'common' )
            //     .field ( 'layout', 'card' )
            //     .field ( 'displayName', 'Common' )
            //         .alternate ( 'ES', 'El Common' )
            //         .alternate ( 'FR', 'Le Common' )
            //     .field ( 'image', 'https://i.imgur.com/VMPKVAN.jpg' )
            //     .field ( 'keywords', 'card common' )
         
            // .definition ( 'rare' )
            //     .field ( 'layout', 'card' )
            //     .field ( 'displayName', 'Rare' )
            //         .alternate ( 'ES', 'El Rare' )
            //         .alternate ( 'FR', 'Le Rare' )
            //     .field ( 'image', 'https://i.imgur.com/BtKggd4.jpg' )
            //     .field ( 'keywords', 'card rare' )
         
            // .definition ( 'ultraRare' )
            //     .field ( 'layout', 'card' )
            //     .field ( 'displayName', 'Ultra-Rare' )
            //         .alternate ( 'ES', 'El Ultra-Rare' )
            //         .alternate ( 'FR', 'Le Ultra-Rare' )
            //     .field ( 'image', 'https://i.imgur.com/2aiJ3cq.jpg' )
            //     .field ( 'keywords', 'card ultra-rare' )

            // //----------------------------------------------------------------//
            // .method ( 'makeRare', 'Combine two commons to make a rare.' )
            //     .assetArg ( 'common0', op.ASSET_TYPE ( 'common' ))
            //     .assetArg ( 'common1', op.ASSET_TYPE ( 'common' ))

            // .method ( 'makeUltraRare', 'Combine two rares to make an ultra-rare.' )
            //     .assetArg ( 'rare0', op.ASSET_TYPE ( 'rare' ))
            //     .assetArg ( 'rare1', op.ASSET_TYPE ( 'rare' ))

            // .method ( 'openPack', 'Open a booster pack.' )
            //     .assetArg ( 'pack', op.ASSET_TYPE ( 'pack' ))

            // .done ()


        let schema = new Schema ();
        await schema.applyTemplate ( template );
        await schema.applyTemplate ( SAMPLE_SCHEMA );

        // const pack0         = schema.addTestAsset ( assets, 'pack', 'bofip-jitut-vupoz-208' );
        // const common0       = schema.addTestAsset ( assets, 'common', 'dosaz-huvuf-nohol-103' );
        // const common1       = schema.addTestAsset ( assets, 'common', 'famaz-havij-zohag-209' );
        // const rare0         = schema.addTestAsset ( assets, 'rare', 'giduv-zotav-domin-184' );
        // const rare1         = schema.addTestAsset ( assets, 'rare', 'honas-simuj-marif-114' );
        // const ultraRare0    = schema.addTestAsset ( assets, 'ultraRare', 'jafoh-najon-gobig-250' );

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
