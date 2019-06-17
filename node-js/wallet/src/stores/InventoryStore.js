/* eslint-disable no-whitespace-before-property */

import { Inventory }            from 'volition-schema-builder';
import { LocalStore }           from './LocalStore';
import { extendObservable }     from "mobx";

//================================================================//
// InventoryStore
//================================================================//
export class InventoryStore extends LocalStore {

    //----------------------------------------------------------------//
    constructor ( accountID, minerURL ) {
        super ();

        this.fetchInventory ( '9090', 'http://localhost:9090' );

        extendObservable ( this, {
            loading: true,
        });
    }

    //----------------------------------------------------------------//
    async fetchInventory ( accountID, minerURL ) {

        try {

            const schemaJSON = await this.revocableFetchJSON ( minerURL + '/schemas/' );

            if ( schemaJSON.schemas ) {

                // just grab the first schema for now
                const schemaTemplate = schemaJSON.schemas [ 0 ];

                const inventoryJSON = await this.revocableFetchJSON ( minerURL + '/accounts/' + accountID + '/inventory' );

                if ( inventoryJSON.inventory ) {
                    
                    let inventory = new Inventory ( schemaTemplate );

                    let assets = inventoryJSON.inventory;
                    for ( let i in assets ) {
                        inventory.addAsset ( assets [ i ]);
                    }

                    inventory.process ();

                    extendObservable ( this, {
                        inventory: inventory,
                    });
                }
            }
            this.loading = false;
        }
        catch ( error ) {
            console.log ( error );
            this.loading = false;
        }
    }
}
