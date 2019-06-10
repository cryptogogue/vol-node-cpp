/* eslint-disable no-whitespace-before-property */

const assert        = require ( 'assert' );
const Schema        = require ( './schema' ).Schema;

//================================================================//
// Inventory
//================================================================//
class Inventory {

    //----------------------------------------------------------------//
    addAsset ( assetID, asset ) {

        let typeName = asset.type;
        if ( !this.assetTypeCounters.hasOwnProperty ( typeName )) {
            this.assetTypeCounters [ typeName ] = 0;
        }

        this.assets [ assetID ] = asset;
        this.assetTypeCounters [ typeName ]++;
    }

    //----------------------------------------------------------------//
    addTestAsset ( typeName ) {

        let count = 0;
        if ( this.assetTypeCounters.hasOwnProperty ( typeName )) {
            count = this.assetTypeCounters [ typeName ];
        }

        let asset = this.schema.newAsset ( typeName );
        assert ( Boolean ( asset ));

        let assetID = `assets.${ typeName }.${ count }`;
        this.addAsset ( assetID, asset );
        return assetID;
    }

    //----------------------------------------------------------------//
    constructor ( schemaTemplate, assets ) {

        if ( !schemaTemplate ) throw 'Provide a valid schema';

        this.schema = new Schema ();
        this.schema.applyTemplate ( schemaTemplate );

        this.assetTypeCounters = {};

        this.assets = {};
        for ( let assetID in assets ) {
            this.addAsset ( assetID, asset );
        }
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

    //----------------------------------------------------------------//
    methodIsValid ( methodName, assetID ) {

        if ( assetID ) {
            let methodBinding = this.methodBindingsByAssetID [ assetID ][ methodName ];
            return methodBinding ? methodBinding.valid : false;
        }
        return this.methodBindingsByName [ methodName ].valid;
    }

    //----------------------------------------------------------------//
    process () {

        let binding = this.schema.processInventory ( this );

        this.methodBindingsByAssetID    = binding.methodBindingsByAssetID;
        this.methodBindingsByName       = binding.methodBindingsByName;
    }
}

var exports = module.exports = {
    Inventory:      Inventory,
}
