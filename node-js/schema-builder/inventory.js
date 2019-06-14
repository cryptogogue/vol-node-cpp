/* eslint-disable no-whitespace-before-property */

const assert        = require ( 'assert' );
const Schema        = require ( './schema' ).Schema;

//================================================================//
// Inventory
//================================================================//
class Inventory {

    //----------------------------------------------------------------//
    addAsset ( asset ) {

        this.assets [ asset.assetID ] = asset;
        this.assetCounter++;
    }

    //----------------------------------------------------------------//
    addTestAsset ( typeName ) {

        let assetID = String ( this.assetCounter );
        this.assetCounter++;

        let asset = this.schema.newAsset ( assetID, typeName );
        assert ( Boolean ( asset ));
        this.addAsset ( asset );

        return assetID;
    }

    //----------------------------------------------------------------//
    addTestAssets ( typeName, count ) {

        for ( let i = 0; i < count; ++i ) {
            this.addTestAsset ( typeName );
        }
    }

    //----------------------------------------------------------------//
    constructor ( schemaTemplate, assets ) {

        if ( !schemaTemplate ) throw 'Provide a valid schema';

        this.schema = new Schema ();
        this.schema.applyTemplate ( schemaTemplate );

        this.assetCounter = 0;
        this.assets = {};

        for ( let assetID in assets ) {
            this.addAsset ( assets [ assetID ]);
        }
    }

    //----------------------------------------------------------------//
    getMethodFormFields ( methodName ) {

        // we'll need the method template (from the schema) *and* the binding
        const methodBinding = this.methodBindingsByName [ methodName ];

        // form fields, by name
        let formFields = {};

        // for each asset field, set the type and the list of qualified assets
        for ( let argname in methodBinding.assetIDsByArgName ) {

            let formField = {
                type: 'asset',
                options: [],
            }

            let assetIDsForArg = methodBinding.assetIDsByArgName [ argname ];
            for ( let i in assetIDsForArg ) {
                let assetID = assetIDsForArg [ i ];
                formField.options.push ( assetID );
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
