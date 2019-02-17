/* eslint-disable no-whitespace-before-property */

const MultiCounter      = require ( './multiCounter' ).MultiCounter;
const squap             = require ( './squap' );

//================================================================//
// SchemaMethod
//================================================================//
class SchemaMethod {

    //----------------------------------------------------------------//
    bindAsset ( schema, assetBinding, methodBinding ) {

        //console.log ( 'BIND ASSET:', this.name, assetBinding.className );

        let opArgs = {
            schema:     schema,
            assets:     [ assetBinding ],
        }

        for ( let argname in this.assetArgs ) {

            if ( this.assetArgs [ argname ].eval ( opArgs )) {

                // console.log ( 'EVAL OK' );

                assetBinding.methodBindings [ this.name ] = methodBinding;
                methodBinding.assetBindings [ argname ].push ( assetBinding );
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
            assetBindings: {},
            valid: false,
        };
        for ( let argname in this.assetArgs ) {
            binding.assetBindings [ argname ] = [];
        }
        return binding;
    }

    //----------------------------------------------------------------//
    validate ( methodBinding ) {

        //console.log ( 'VALIDATE', methodBinding );

        const assetArgs = this.assetArgs;

        const nArgs = Object.keys ( assetArgs ).length;
        let multiCounter = new MultiCounter ( nArgs );
        let indexedArgNames = [];
        let indexedArgLists = [];
        
        // build the tables and initialize the counter.
        for ( let argname in assetArgs ) {

            const argList = methodBinding.assetBindings [ argname ];
            if ( argList.length === 0 ) return; // bail; method cannever be valid.

            multiCounter.setLimit ( indexedArgNames.length, argList.length );
            indexedArgNames.push ( argname );
            indexedArgLists.push ( argList );
        }

        // TODO: we eventually need to prune our invalid asset bindings

        // try every permutation of args until a valid configuration is found.
        for ( ; multiCounter.cycles < 1; multiCounter.increment ()) {

            let visited = {}; // object to hold visited asset args.
            let passed = 0;

            for ( let i = 0; i < nArgs; ++i, ++passed ) {

                //const argname = indexedArgNames [ i ];
                const argList       = indexedArgLists [ i ];
                const assetBinding  = argList [ multiCounter.count ( i )];
                const className     = assetBinding.className;

                if ( !( assetBinding.className in visited )) {
                    visited [ className ] = 0;
                }
                const visitedCount = visited [ className ];

                // TODO: add more early outs.

                // if we've used up all the asset type, bail.
                if ( visitedCount >= assetBinding.quantity ) break;

                visited [ className ] = visitedCount + 1;
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

        //console.log ( 'APPLYING SCHEMA TEMPLATE:', template.name );

        const name = template.name;

        if ( this.applied [ name ]) return;
        this.applied [ name ] = true;

        // build an asset table for quick reference
        for ( let className in template.assetDefinitions ) {

            const definition = template.assetDefinitions [ className ];

            // TODO: illegal for assets to overwrite each other
            this.assets [ className ] = {
                template:       template.assetTemplates [ definition.extends ],
                definition:     definition,
            }
        }

        for ( let methodName in template.methods ) {
            this.methods [ methodName ] = new SchemaMethod ( methodName, template.methods [ methodName ]);
        }
    }

    //----------------------------------------------------------------//
    checkItem ( itemName, specialization ) {

        // TODO: write me
        return true;
    }

    //----------------------------------------------------------------//
    constructor () {

        this.applied = {};  // table of schema names that have already been applied
        this.methods = {};  // table of all available methods
        this.assets = {};   // table of all known asset types
    }

    //----------------------------------------------------------------//
    getField ( className, fieldName ) {

        if ( className in this.assets ) {
            return this.assets [ className ].definition.fields [ fieldName ];

            // TODO: handle inheritance
        }
        return undefined;
    }

    //----------------------------------------------------------------//
    isType ( className, other ) {

        //console.log ( 'IS TYPE', className, other );
        //return true;

        if ( className in this.assets ) {
            if ( className === other ) return true;

            // TODO: handle inheritance
        }
        return false;
    }

    //----------------------------------------------------------------//
    processInventory ( inventory ) {

        // for each item
        //     has the item been consumed?
        //     what are all the possible recipes?

        // for each recipe
        //     what is the set of items yielding at least one valid outcome?
        //     item may qualify as one or more params
        //     for each param, list of qualified items


        // distibute all items to recipes that they qualify for
        // add each active recipe to each item
        // process each recipe to determine if it is active

        // recalculate when items are consumed (will change other recipes)
        // inventory items identified by reference?

        //console.log ( 'PROCESS INVENTORY' );

        let binding = {
            assetBindings: {},
            methodBindings: {},
        }

        // generate all the empty asset bindings.
        for ( let className in inventory.assets ) {

            const asset = inventory.assets [ className ];

            binding.assetBindings [ className ] = {
                className: className,
                quantity: asset.quantity,
                methodBindings: {},
            };
        }

        // generate all the empty method bindings.
        for ( let methodName in this.methods ) {

            binding.methodBindings [ methodName ] = this.methods [ methodName ].newBinding ();
        }

        // bind each asset and each method...
        for ( let assetType in inventory.assets ) {
            for ( let methodName in this.methods ) {
                this.methods [ methodName ].bindAsset (
                    this,
                    binding.assetBindings [ assetType ],
                    binding.methodBindings [ methodName ]
                );
            }
        }

        // at this stage, assets are populated and linked to methods.
        // all methods and method params track assets that qualify.
        // now we have to iterate through all the methods and find out if they can be executed.
        for ( let methodName in this.methods ) {

            // create a relationship if the asset qualifies.
            this.methods [ methodName ].validate ( binding.methodBindings [ methodName ]);
        }

        return binding;
    }
}

var exports = module.exports = {
    Schema:     Schema,
}
