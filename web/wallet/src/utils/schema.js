/* eslint-disable no-whitespace-before-property */

import { MultiCounter }         from './multiCounter';
import { makeSquap }            from './squap';

//================================================================//
// SchemaMethod
//================================================================//
class SchemaMethod {

    //----------------------------------------------------------------//
    bindAsset ( assetBinding, methodBinding ) {

        console.log ( 'BIND ASSET:', this.name, assetBinding.className );

        for ( let argname in this.assetArgs ) {

            if ( this.assetArgs [ argname ].eval ( assetBinding )) {
                assetBinding.qualifiedMethods [ this.name ] = methodBinding;
                methodBinding.qualifiedAssets [ argname ].push ( assetBinding );
            }
        }
    }

    //----------------------------------------------------------------//
    constructor ( name, template ) {

        console.log ( 'SCHEMA METHOD:', name );

        this.name = name;
        this.assetArgs = {};
        this.constArgs = {};
        this.constraints = {};

        for ( let argname in template.assetArgs ) {
            this.assetArgs [ argname ] = makeSquap ( template.assetArgs [ argname ]);
        }

        for ( let argname in template.constArgs ) {
            this.constArgs [ argname ] = makeSquap ( template.constArgs [ argname ]);
        }

        // TODO: constraints
        console.log ( this )
    }

    //----------------------------------------------------------------//
    newBinding () {

        let binding = {
            qualifiedAssets: {},
            valid: false,
        };
        for ( let argname in this.assetArgs ) {
            binding.qualifiedAssets [ argname ] = [];
        }
        return binding;
    }

    //----------------------------------------------------------------//
    validate ( methodBinding ) {

        console.log ( 'VALIDATE', methodBinding );

        const assetArgs = this.assetArgs;

        const nArgs = Object.keys ( assetArgs ).length;
        let multiCounter = new MultiCounter ( nArgs );
        let indexedArgNames = [];
        let indexedArgLists = [];
        
        // build the tables and initialize the counter.
        for ( let argname in assetArgs ) {

            const argList = methodBinding.qualifiedAssets [ argname ];
            if ( argList.length === 0 ) return; // bail; method cannever be valid.

            multiCounter.setLimit ( indexedArgNames.length, argList.length );
            indexedArgNames.push ( argname );
            indexedArgLists.push ( argList );
        }

        // TODO: we eventually need to prune our invalid asset bindings

        // try every permutation of args until a valid configuration is found.
        for ( ; multiCounter.cycles < 1; multiCounter.increment ()) {

            console.log ( 'HELLO' );

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
                console.log ( 'IT IS VALID!' );
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

        console.log ( 'APPLYING SCHEMA TEMPLATE:', template.name );

        const name = template.name;

        if ( this.applied [ name ]) return;
        this.applied [ name ] = true;

        for ( let methodName in template.methods ) {
            this.methods [ methodName ] = new SchemaMethod ( methodName, template.methods [ methodName ]);
        }
    }

    //----------------------------------------------------------------//
    constructor () {

        this.applied = {};  // table of schema names that have already been applied
        this.methods = {};  // table of all available methods
        this.assets = {};   // table of all known asset types
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

        console.log ( 'PROCESS INVENTORY' );

        let binding = {
            assets: [],
            methods: {},
        }

        // generate all the empty asset bindings.
        for ( let i in inventory.assets ) {

            const asset = inventory.assets [ i ];

            binding.assets [ i ] = {
                className: asset.className,
                quantity: asset.quantity,
                qualifiedMethods: {},
            };
        }

        console.log ( 'HELLO' );

        // generate all the empty method bindings.
        for ( let methodName in this.methods ) {

            binding.methods [ methodName ] = this.methods [ methodName ].newBinding ();
        }

        // for each asset.
        for ( let i in inventory.assets ) {
            const assetBinding = binding.assets [ i ];

            // for each method.
            for ( let methodName in this.methods ) {
                let methodBinding = binding.methods [ methodName ];

                // bind.
                this.methods [ methodName ].bindAsset ( assetBinding, methodBinding );
            }
        }

        // at this stage, assets are populated and linked to methods.
        // all methods and method params track assets that qualify.
        // now we have to iterate through all the methods and find out if they can be executed.
        for ( let methodName in this.methods ) {

            // create a relationship if the asset qualifies.
            this.methods [ methodName ].validate ( binding.methods [ methodName ]);
        }

        return binding;
    }
}

export { Schema };
