/* eslint-disable no-whitespace-before-property */

const Inventory         = require ( './inventory' ).Inventory;
const Schema            = require ( './schema' ).Schema;

const schemaBuilder     = require ( './schema-builder' );
const op                = schemaBuilder.op;

//----------------------------------------------------------------//
test ( 'define schema', () => {

    let schemaTemplate = schemaBuilder ( 'TEST_SCHEMA', 'schema.lua' )

        //----------------------------------------------------------------//
        .assetTemplate ( 'base' )
            .field ( 'displayName' ).string ()
     
        .assetTemplate ( 'card' ).extends ( 'base' )
            .field ( 'keywords' ).string ().array ()

        //----------------------------------------------------------------//
        .assetDefinition ( 'pack', 'base' )
            .field ( 'displayName', 'Booster Pack' )
     
        .assetDefinition ( 'common', 'card' )
            .field ( 'displayName', 'Common' )
            .field ( 'keywords', [ 'card', 'common' ])
     
        .assetDefinition ( 'rare', 'card' )
            .field ( 'displayName', 'Rare' )
            .field ( 'keywords', [ 'card', 'rare' ])
     
        .assetDefinition ( 'ulraRare', 'card' )
            .field ( 'displayName', 'Ultra-Rare' )
            .field ( 'keywords', [ 'card', 'ultra-rare' ])

        //----------------------------------------------------------------//
        .method ( 'makeRare', 1, 2, 'Combine two commons to make a rare.' )
            .assetArg ( 'common0', op.ASSET_TYPE ( 'common' ))
            .assetArg ( 'common1', op.ASSET_TYPE ( 'common' ))

        .method ( 'makeUltraRare', 1, 2, 'Combine two rares to make an ultra-rare.' )
            .assetArg ( 'rare0', op.ASSET_TYPE ( 'rare' ))
            .assetArg ( 'rare1', op.ASSET_TYPE ( 'rare' ))

        .method ( 'openPack', 1, 2, 'Open a booster pack.' )
            .assetArg ( 'pack', op.ASSET_TYPE ( 'pack' ))

        .done ()

    let schema = new Schema ();
    schema.applyTemplate ( schemaTemplate );

    let inventory = new Inventory ( schema );

    inventory.addItem ( 'pack', 1 );
    inventory.addItem ( 'common', 2 );
    inventory.addItem ( 'rare', 2 );
    inventory.addItem ( 'ultraRare', 1 );

    // test binding analysis
    let binding = schema.processInventory ( inventory );

    // all of the schema methods should be valid for this inventory.
    expect ( binding.methodBindings [ 'makeRare' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'makeUltraRare' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'openPack' ].valid ).toBe ( true );

    // 'makeRare' only valid on two commons.
    expect ( 'makeRare' in binding.assetBindings [ 'pack' ].methodBindings ).toBe ( false );
    expect ( 'makeRare' in binding.assetBindings [ 'common' ].methodBindings ).toBe ( true );
    expect ( 'makeRare' in binding.assetBindings [ 'rare' ].methodBindings ).toBe ( false );
    expect ( 'makeRare' in binding.assetBindings [ 'ultraRare' ].methodBindings ).toBe ( false );

    // 'makeUltraRare' only valid on two rares.
    expect ( 'makeUltraRare' in binding.assetBindings [ 'pack' ].methodBindings ).toBe ( false );
    expect ( 'makeUltraRare' in binding.assetBindings [ 'common' ].methodBindings ).toBe ( false );
    expect ( 'makeUltraRare' in binding.assetBindings [ 'rare' ].methodBindings ).toBe ( true );
    expect ( 'makeUltraRare' in binding.assetBindings [ 'ultraRare' ].methodBindings ).toBe ( false );

    // 'openPack' only valid on packs.
    expect ( 'openPack' in binding.assetBindings [ 'pack' ].methodBindings ).toBe ( true );
    expect ( 'openPack' in binding.assetBindings [ 'common' ].methodBindings ).toBe ( false );
    expect ( 'openPack' in binding.assetBindings [ 'rare' ].methodBindings ).toBe ( false );
    expect ( 'openPack' in binding.assetBindings [ 'ultraRare' ].methodBindings ).toBe ( false );
});

//----------------------------------------------------------------//
test ( 'test operators', () => {

    let schemaTemplate = schemaBuilder ( 'TEST_SCHEMA', 'schema.lua' )

        //----------------------------------------------------------------//
        .assetTemplate ( 'testTemplate' )
            .field ( 'numberField' ).numeric ()
            .field ( 'stringField' ).string ()
            .field ( 'numberArrayField' ).numeric ().array ()
            .field ( 'stringArrayField' ).string ().array ()

        //----------------------------------------------------------------//
        .assetDefinition ( 'testAsset', 'testTemplate' )
            .field ( 'numberField', 123 )
            .field ( 'stringField', 'oneTwoThree' )
            .field ( 'numberArrayField', [ 1, 2, 3 ])
            .field ( 'stringArrayField', [ 'one', 'two', 'three' ])

        //----------------------------------------------------------------//
        .method ( 'assetType', 1, 2, '' )
            .assetArg ( 'arg0', op.ASSET_TYPE ( 'testAsset' ))

        //----------------------------------------------------------------//
        .method ( 'numericEqual', 1, 2, '' )
            .assetArg ( 'arg0', op.EQUAL ( op.FIELD ( 'numberField' ), 123 ))

        .method ( 'stringEqual', 1, 2, '' )
            .assetArg ( 'arg0', op.EQUAL ( op.FIELD ( 'stringField' ), 'oneTwoThree' ))

        .method ( 'numericNotEqual', 1, 2, '' )
            .assetArg ( 'arg0', op.NOT_EQUAL ( op.FIELD ( 'numberField' ), 321 ))

        .method ( 'stringNotEqual', 1, 2, '' )
            .assetArg ( 'arg0', op.NOT_EQUAL ( op.FIELD ( 'stringField' ), 'threeTwoOne' ))

        .method ( 'numericGreater', 1, 2, '' )
            .assetArg ( 'arg0', op.GREATER ( op.FIELD ( 'numberField' ), 100 ))

        .method ( 'numericGreaterOrEqual', 1, 2, '' )
            .assetArg ( 'arg0', op.GREATER_OR_EQUAL ( op.FIELD ( 'numberField' ), 123 ))

        .method ( 'numericLess', 1, 2, '' )
            .assetArg ( 'arg0', op.LESS ( op.FIELD ( 'numberField' ), 200 ))

        .method ( 'numericLessOrEqual', 1, 2, '' )
            .assetArg ( 'arg0', op.LESS_OR_EQUAL ( op.FIELD ( 'numberField' ), 123 ))

        .method ( 'numericIn', 1, 2, '' )
            .assetArg ( 'arg0', op.IN ( op.FIELD ( 'numberArrayField' ), 2 ))

        .method ( 'stringIn', 1, 2, '' )
            .assetArg ( 'arg0', op.IN ( op.FIELD ( 'stringArrayField' ), 'two' ))

        .method ( 'logicalAnd', 1, 2, '' )
            .assetArg ( 'arg0', op.AND (
                op.EQUAL ( op.FIELD ( 'numberField' ), 123 ),                   // true
                op.EQUAL ( op.FIELD ( 'stringField' ), 'oneTwoThree' )          // true
            )
        )
        .method ( 'logicalNotAnd', 1, 2, '' )
            .assetArg ( 'arg0', op.NOT (
                op.AND (
                    op.EQUAL ( op.FIELD ( 'numberField' ), 123 ),               // true
                    op.EQUAL ( op.FIELD ( 'stringField' ), 'threeTwoOne' )      // false
                )
            )
        )
        .method ( 'logicalOr', 1, 2, '' )
            .assetArg ( 'arg0', op.OR (
                op.EQUAL ( op.FIELD ( 'numberField' ), 123 ),                   // true
                op.EQUAL ( op.FIELD ( 'stringField' ), 'threeTwoOne' )          // false
            )
        )
        .method ( 'logicalNot', 1, 2, '' )
            .assetArg ( 'arg0', op.NOT ( op.EQUAL ( op.FIELD ( 'numberField' ), 321 )))

        .method ( 'logicalXor', 1, 2, '' )
            .assetArg ( 'arg0', op.XOR (
                op.EQUAL ( op.FIELD ( 'numberField' ), 123 ),                   // true
                op.EQUAL ( op.FIELD ( 'stringField' ), 'threeTwoOne' )          // false
            )
        )
        .method ( 'logicalNotXor', 1, 2, '' )
            .assetArg ( 'arg0', op.NOT (
                op.XOR (
                    op.EQUAL ( op.FIELD ( 'numberField' ), 123 ),               // true
                    op.EQUAL ( op.FIELD ( 'stringField' ), 'oneTwoThree' )      // true
                )
            )
        )

        .method ( 'add', 1, 2, '' )
            .assetArg ( 'arg0', op.EQUAL ( op.ADD ( op.FIELD ( 'numberField' ), 2 ), 125 ))

        .method ( 'div', 1, 2, '' )
            .assetArg ( 'arg0', op.EQUAL ( op.DIV ( op.FIELD ( 'numberField' ), 2 ), 61.5 ))

        .method ( 'mod', 1, 2, '' )
            .assetArg ( 'arg0', op.EQUAL ( op.MOD ( op.FIELD ( 'numberField' ), 120 ), 3 ))

        .method ( 'mul', 1, 2, '' )
            .assetArg ( 'arg0', op.EQUAL ( op.MUL ( op.FIELD ( 'numberField' ), 2 ), 246 ))

        .method ( 'sub', 1, 2, '' )
            .assetArg ( 'arg0', op.EQUAL ( op.SUB ( op.FIELD ( 'numberField' ), 3 ), 120 ))

        .done ()

    // console.log ( JSON.stringify ( schemaTemplate, null, 4 ));

    let schema = new Schema ();
    schema.applyTemplate ( schemaTemplate );

    let inventory = new Inventory ( schema );
    inventory.addItem ( 'testAsset', 1 );

    // test binding analysis
    let binding = schema.processInventory ( inventory );

    // all of the schema methods should be valid for this inventory.
    expect ( binding.methodBindings [ 'assetType' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'numericEqual' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'stringEqual' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'numericNotEqual' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'stringNotEqual' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'numericGreater' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'numericGreaterOrEqual' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'numericLess' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'numericLessOrEqual' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'numericIn' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'stringIn' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'logicalAnd' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'logicalNotAnd' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'logicalOr' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'logicalNot' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'logicalXor' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'logicalNotXor' ].valid ).toBe ( true );

    expect ( binding.methodBindings [ 'add' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'div' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'mod' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'mul' ].valid ).toBe ( true );
    expect ( binding.methodBindings [ 'sub' ].valid ).toBe ( true );

    expect ( 'assetType' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericEqual' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'stringEqual' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericNotEqual' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'stringNotEqual' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericGreater' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericGreaterOrEqual' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericLess' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericLessOrEqual' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericIn' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'stringIn' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalAnd' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalNotAnd' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalOr' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalNot' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalXor' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalNotXor' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );

    expect ( 'add' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'div' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'mod' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'mul' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'sub' in binding.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
});
