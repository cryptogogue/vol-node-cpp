/* eslint-disable no-whitespace-before-property */

const Schema            = require ( './schema' ).Schema;

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
    constructor ( schemaTemplate, assets, specializations ) {

        if ( !schemaTemplate ) throw 'Provide a valid schema';

        this.schema = new Schema ();
        this.schema.applyTemplate ( schemaTemplate );

        this.assets = {};
        for ( let i in assets ) {
            let asset = assets [ i ];
            this.addItem ( asset.className, asset.quantity );
        }

        let binding = this.schema.processInventory ( this );

        console.log ( 'BINDING:', binding );

        this.methodBindings = binding.methodBindings;
        this.assetBindings = binding.assetBindings;
    }

    //----------------------------------------------------------------//
    getMethodFormFields ( methodName ) {

        // we'll need the method template (from the schema) *and* the binding
        const methodBinding = this.methodBindings [ methodName ];

        // form fields, by name
        let formFields = {};

        // for each asset field, set the type and the list of qualified assets
        for ( let argname in methodBinding.assetBindings ) {

            let formField = {
                type: 'asset',
                options: [],
            }

            let assetBindings = methodBinding.assetBindings [ argname ];
            for ( let i in assetBindings ) {
                formField.options.push ( assetBindings [ i ].className );
            }

            formFields [ argname ] = formField;
        }
        return formFields;
    }
}

var exports = module.exports = {
    Inventory:      Inventory,
}
