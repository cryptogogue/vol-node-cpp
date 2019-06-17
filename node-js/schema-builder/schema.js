/* eslint-disable no-whitespace-before-property */

const assert            = require ( 'assert' );
const MultiCounter      = require ( './multiCounter' ).MultiCounter;
const squap             = require ( './squap' );

//================================================================//
// SchemaMethod
//================================================================//
class SchemaMethod {

    //----------------------------------------------------------------//
    bindAsset ( schema, asset, methodBinding, methodBindingsForAssetByMethodName ) {

        let opArgs = {
            schema:     schema,
            assets:     [ asset ],
        }

        for ( let argname in this.assetArgs ) {

            if ( this.assetArgs [ argname ].eval ( opArgs )) {

                // console.log ( 'EVAL OK' );
                methodBinding.assetIDsByArgName [ argname ].push ( asset.assetID );
                methodBindingsForAssetByMethodName [ this.name ] = methodBinding;
            }
        }
    }

    //----------------------------------------------------------------//
    constructor ( name, template ) {

        //console.log ( 'SCHEMA METHOD:', name );

        this.name = name;
        this.assetArgs = {};
        this.constArgs = {};
        this.constraints = {};

        for ( let argname in template.assetArgs ) {
            this.assetArgs [ argname ] = squap.makeSquap ( template.assetArgs [ argname ]);
        }

        for ( let argname in template.constArgs ) {
            this.constArgs [ argname ] = squap.makeSquap ( template.constArgs [ argname ]);
        }

        // TODO: constraints
        //console.log ( this )
    }

    //----------------------------------------------------------------//
    newBinding () {

        let binding = {
            assetIDsByArgName: {},
            valid: false,
        };
        for ( let argname in this.assetArgs ) {
            binding.assetIDsByArgName [ argname ] = [];
        }
        return binding;
    }

    //----------------------------------------------------------------//
    validate ( methodBinding ) {

        //console.log ( 'VALIDATE', methodBinding );

        const assetArgs = this.assetArgs;

        const nArgs = Object.keys ( assetArgs ).length;
        let multiCounter = new MultiCounter ( nArgs );
        let argListsByIndex = [];
        let argCount = 0;
        
        // build the tables and initialize the counter.
        for ( let argName in assetArgs ) {

            const argList = methodBinding.assetIDsByArgName [ argName ];
            if ( argList.length === 0 ) return; // bail; method can never be valid.

            multiCounter.setLimit ( argCount, argList.length );
            argListsByIndex.push ( argList );
            argCount++
        }

        // TODO: we eventually need to prune out invalid asset bindings

        // try every permutation of args until a valid configuration is found.
        for ( ; multiCounter.cycles < 1; multiCounter.increment ()) {

            let visitedAssetIDs = {}; // object to hold visited asset args.
            let passed = 0;

            for ( let i = 0; i < nArgs; ++i, ++passed ) {

                //const argname = indexedArgNames [ i ];
                const argList       = argListsByIndex [ i ];
                const assetID       = argList [ multiCounter.count ( i )];

                if ( visitedAssetIDs [ assetID ] === true ) break;
                visitedAssetIDs [ assetID ] = true;
            }

            if ( passed === nArgs ) {
                // TODO: evaluate constraints
                // TODO: this early out is a hack for now
                // console.log ( 'IT IS VALID!' );
                methodBinding.valid = true;
            }
        }
    }
}

//================================================================//
// Schema
//================================================================//
class Schema {

    //----------------------------------------------------------------//
    applyTemplate ( template ) {

        if ( !template ) return;

        const name = template.name;

        if ( this.applied [ name ]) return;
        this.applied [ name ] = true;

        // build an asset table for quick reference
        for ( let typeName in template.definitions ) {
            this.definitions [ typeName ] = template.definitions [ typeName ]; // TODO: deep copy
        }

        for ( let methodName in template.methods ) {
            this.methods [ methodName ] = new SchemaMethod ( methodName, template.methods [ methodName ]);
        }
    }

    //----------------------------------------------------------------//
    constructor () {

        this.applied        = {}; // table of schema names that have already been applied
        this.methods        = {}; // table of all available methods
        this.definitions    = {}; // table of all known asset types
    }

    //----------------------------------------------------------------//
    newAsset ( assetID, typeName ) {

        let definition = this.definitions [ typeName ];
        assert ( Boolean ( definition ));

        let asset = {
            type:       typeName,
            assetID:    assetID,
            fields:     {},
        };

        for ( let fieldName in definition.fields ) {
            let field = definition.fields [ fieldName ];
            asset.fields [ fieldName ] = field.value;
        }
        return asset;
    }

    //----------------------------------------------------------------//
    processInventory ( inventory ) {

        let binding = {
            methodBindingsByAssetID: {},
            methodBindingsByName: {},
        }

        // generate all the empty method bindings.
        for ( let methodName in this.methods ) {
            binding.methodBindingsByName [ methodName ] = this.methods [ methodName ].newBinding ();
        }

        // bind each asset and each method...
        for ( let assetID in inventory.assets ) {

            binding.methodBindingsByAssetID [ assetID ] = {};

            for ( let methodName in this.methods ) {

                this.methods [ methodName ].bindAsset (
                    this,
                    inventory.assets [ assetID ],
                    binding.methodBindingsByName [ methodName ],
                    binding.methodBindingsByAssetID [ assetID ]
                );
            }
        }

        // at this stage, assets are populated and linked to methods.
        // all methods and method params track assets that qualify.
        // now we have to iterate through all the methods and find out if they can be executed.
        for ( let methodName in this.methods ) {

            // create a relationship if the asset qualifies.
            this.methods [ methodName ].validate ( binding.methodBindingsByName [ methodName ]);
        }

        return binding;
    }
}

var exports = module.exports = {
    Schema:     Schema,
}
