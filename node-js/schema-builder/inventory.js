/* eslint-disable no-whitespace-before-property */

//================================================================//
// Inventory
//================================================================//
class Inventory {

    //----------------------------------------------------------------//
    addItem ( className, quantity, specialization ) {

        this.schema.checkItem ( className, specialization );

        if ( !( className in this.assets )) {
            this.assets [ className ] = {
                quantity:           0,
                specializations:    [],
            }
        }

        const asset = this.assets [ className ];
        asset.quantity += quantity;
        if ( specialization ) {
            asset.specializations.push ( specialization );
        }
    }

    //----------------------------------------------------------------//
    constructor ( schema ) {

        if ( !schema ) throw 'Provide a valid schema';

        this.schema = schema;
        this.assets = {};
    }
}

var exports = module.exports = {
    Inventory:      Inventory,
}
