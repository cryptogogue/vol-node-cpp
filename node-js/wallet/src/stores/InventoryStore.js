/* eslint-disable no-whitespace-before-property */

import { Inventory }                                from 'volition-schema-builder';
import { LocalStore }                               from './LocalStore';
import { action, extendObservable, observable }     from "mobx";

//================================================================//
// InventoryStore
//================================================================//
export class InventoryStore extends LocalStore {

    @observable loading = true;
    @observable inventory = new Inventory ();

    //----------------------------------------------------------------//
    @action
    applyTemplate ( template ) {

        this.inventory.applyTemplate ( template );
    }

    //----------------------------------------------------------------//
    constructor ( accountID, minerURL ) {
        super ();

        this.fetchInventory ( '9090', 'http://localhost:9090' );
    }

    //----------------------------------------------------------------//
    async fetchInventory ( accountID, minerURL ) {

        try {

            const schemaJSON = await this.revocableFetchJSON ( minerURL + '/schemas/' );

            if ( schemaJSON.schemas ) {

                for ( let i in schemaJSON.schemas ) {
                    this.applyTemplate ( schemaJSON.schemas [ i ]);
                }

                const inventoryJSON = await this.revocableFetchJSON ( minerURL + '/accounts/' + accountID + '/inventory' );

                this.updateInventory ( inventoryJSON.inventory ); 
            }
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
    @action
    updateInventory ( assets ) {

        if ( !assets ) return;

        for ( let i in assets ) {
            this.inventory.addAsset ( assets [ i ]);
        }
        this.inventory.process ();
    }
}
