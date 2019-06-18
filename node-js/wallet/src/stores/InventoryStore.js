/* eslint-disable no-whitespace-before-property */

import { Binding, Schema }                                  from 'volition-schema-builder';
import { LocalStore }                                       from './LocalStore';
import { action, computed, extendObservable, observable }   from "mobx";

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

        this.fetchInventory ( accountID, minerURL );
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
    finishLoading ( template, assets ) {

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
