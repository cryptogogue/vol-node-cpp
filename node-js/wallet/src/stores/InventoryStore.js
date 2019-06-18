/* eslint-disable no-whitespace-before-property */

import { Binding, Schema, buildSchema }                     from 'volition-schema-builder';
import { LocalStore }                                       from './LocalStore';
import { action, computed, extendObservable, observable }   from "mobx";

const DEBUG = true;

//================================================================//
// InventoryStore
//================================================================//
export class InventoryStore extends LocalStore {

    @observable loading     = true;

    @observable assets      = {};
    @observable schema      = new Schema (); // empty schema
    @observable binding     = new Binding (); // empty binding

    //----------------------------------------------------------------//
    constructor ( accountID, minerURL ) {
        super ();

        if ( DEBUG || ( !( accountID && minerURL ))) {
            this.useDebugInventory ();
        }
        else {
            this.fetchInventory ( accountID, minerURL );
        }
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
    @action
    update ( templates, assets ) {

        if ( templates ) {
            this.schema = new Schema ();
            for ( let i in templates ) {
                this.schema.applyTemplate ( templates [ i ]);
            }
        }

        if ( assets ) {
            this.assets = assets;
        }

        this.binding = this.schema.generateBinding ( this.assets );
    }

    //----------------------------------------------------------------//
    @action
    useDebugInventory () {

        const op = buildSchema.op;

        let schemaTemplate = buildSchema ( 'TEST_SCHEMA', 'schema.lua' )

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

        let schema = new Schema ( schemaTemplate );

        let assets = {};

        const pack0         = schema.addTestAsset ( assets, 'pack' );
        const common0       = schema.addTestAsset ( assets, 'common' );
        const common1       = schema.addTestAsset ( assets, 'common' );
        const rare0         = schema.addTestAsset ( assets, 'rare' );
        const rare1         = schema.addTestAsset ( assets, 'rare' );
        const ultraRare0    = schema.addTestAsset ( assets, 'ultraRare' );

        this.schema = schema;
        this.assets = assets;
        this.binding = schema.generateBinding ( assets );

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
