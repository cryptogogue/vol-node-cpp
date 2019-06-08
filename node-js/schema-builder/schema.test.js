/* eslint-disable no-whitespace-before-property */

const Inventory         = require ( './inventory' ).Inventory;

const schemaBuilder     = require ( './schema-builder' );
const op                = schemaBuilder.op;

//----------------------------------------------------------------//
test ( 'define schema', () => {

    let schemaTemplate = schemaBuilder ( 'TEST_SCHEMA', 'schema.lua' )

        //----------------------------------------------------------------//
        .assetTemplate ( 'base' )
            .field ( 'displayName' ).string ()
     
        .assetTemplate ( 'card' ).extends ( 'base' )
            .field ( 'keywords' ).string ()

        //----------------------------------------------------------------//
        .assetDefinition ( 'pack', 'base' )
            .field ( 'displayName', 'Booster Pack' )
     
        .assetDefinition ( 'common', 'card' )
            .field ( 'displayName', 'Common' )
            .field ( 'keywords', 'card common' )
     
        .assetDefinition ( 'rare', 'card' )
            .field ( 'displayName', 'Rare' )
            .field ( 'keywords', 'card rare' )
     
        .assetDefinition ( 'ulraRare', 'card' )
            .field ( 'displayName', 'Ultra-Rare' )
            .field ( 'keywords', 'card ultra-rare' )

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

    let assets = [
        { className: 'pack', quantity: 1 },
        { className: 'common', quantity: 2 },
        { className: 'rare', quantity: 2 },
        { className: 'ultraRare', quantity: 1 },
    ];

    let inventory = new Inventory ( schemaTemplate, assets );

    // test binding analysis

    // all of the schema methods should be valid for this inventory.
    expect ( inventory.methodBindings [ 'makeRare' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'makeUltraRare' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'openPack' ].valid ).toBe ( true );

    // 'makeRare' only valid on two commons.
    expect ( 'makeRare' in inventory.assetBindings [ 'pack' ].methodBindings ).toBe ( false );
    expect ( 'makeRare' in inventory.assetBindings [ 'common' ].methodBindings ).toBe ( true );
    expect ( 'makeRare' in inventory.assetBindings [ 'rare' ].methodBindings ).toBe ( false );
    expect ( 'makeRare' in inventory.assetBindings [ 'ultraRare' ].methodBindings ).toBe ( false );

    // 'makeUltraRare' only valid on two rares.
    expect ( 'makeUltraRare' in inventory.assetBindings [ 'pack' ].methodBindings ).toBe ( false );
    expect ( 'makeUltraRare' in inventory.assetBindings [ 'common' ].methodBindings ).toBe ( false );
    expect ( 'makeUltraRare' in inventory.assetBindings [ 'rare' ].methodBindings ).toBe ( true );
    expect ( 'makeUltraRare' in inventory.assetBindings [ 'ultraRare' ].methodBindings ).toBe ( false );

    // 'openPack' only valid on packs.
    expect ( 'openPack' in inventory.assetBindings [ 'pack' ].methodBindings ).toBe ( true );
    expect ( 'openPack' in inventory.assetBindings [ 'common' ].methodBindings ).toBe ( false );
    expect ( 'openPack' in inventory.assetBindings [ 'rare' ].methodBindings ).toBe ( false );
    expect ( 'openPack' in inventory.assetBindings [ 'ultraRare' ].methodBindings ).toBe ( false );
});

//----------------------------------------------------------------//
test ( 'test operators', () => {

    let schemaTemplate = schemaBuilder ( 'TEST_SCHEMA', 'schema.lua' )

        //----------------------------------------------------------------//
        .assetTemplate ( 'testTemplate' )
            .field ( 'numberField' ).numeric ()
            .field ( 'stringField' ).string ()

        //----------------------------------------------------------------//
        .assetDefinition ( 'testAsset', 'testTemplate' )
            .field ( 'numberField', 123 )
            .field ( 'stringField', 'oneTwoThree' )

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

    let assets = [
        { className: 'testAsset', quantity: 1 },
    ];

    let inventory = new Inventory ( schemaTemplate, assets );

    // test binding analysis

    // all of the schema methods should be valid for this inventory.
    expect ( inventory.methodBindings [ 'assetType' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'numericEqual' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'stringEqual' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'numericNotEqual' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'stringNotEqual' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'numericGreater' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'numericGreaterOrEqual' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'numericLess' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'numericLessOrEqual' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'logicalAnd' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'logicalNotAnd' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'logicalOr' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'logicalNot' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'logicalXor' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'logicalNotXor' ].valid ).toBe ( true );

    expect ( inventory.methodBindings [ 'add' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'div' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'mod' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'mul' ].valid ).toBe ( true );
    expect ( inventory.methodBindings [ 'sub' ].valid ).toBe ( true );

    expect ( 'assetType' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericEqual' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'stringEqual' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericNotEqual' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'stringNotEqual' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericGreater' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericGreaterOrEqual' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericLess' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'numericLessOrEqual' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalAnd' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalNotAnd' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalOr' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalNot' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalXor' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'logicalNotXor' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );

    expect ( 'add' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'div' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'mod' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'mul' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
    expect ( 'sub' in inventory.assetBindings [ 'testAsset' ].methodBindings ).toBe ( true );
});
