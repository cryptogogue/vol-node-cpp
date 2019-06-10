/* eslint-disable no-whitespace-before-property */

const Inventory         = require ( './inventory' ).Inventory;

const schemaBuilder     = require ( './schema-builder' );
const op                = schemaBuilder.op;

//----------------------------------------------------------------//
test ( 'define schema', () => {

    let schemaTemplate = schemaBuilder ( 'TEST_SCHEMA', 'schema.lua' )

        //----------------------------------------------------------------//
        .definition ( 'pack' )
            .field ( 'displayName', 'Booster Pack' )
     
        .definition ( 'common' )
            .field ( 'displayName', 'Common' )
            .field ( 'keywords', 'card common' )
     
        .definition ( 'rare' )
            .field ( 'displayName', 'Rare' )
            .field ( 'keywords', 'card rare' )
     
        .definition ( 'ultraRare' )
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

    let inventory = new Inventory ( schemaTemplate );

    const pack0         = inventory.addTestAsset ( 'pack' );
    const common0       = inventory.addTestAsset ( 'common' );
    const common1       = inventory.addTestAsset ( 'common' );
    const rare0         = inventory.addTestAsset ( 'rare' );
    const rare1         = inventory.addTestAsset ( 'rare' );
    const ultraRare0    = inventory.addTestAsset ( 'ultraRare' );

    inventory.process ();

    // test binding analysis

    // all of the schema methods should be valid for this inventory.
    expect ( inventory.methodIsValid ( 'makeRare' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'makeUltraRare' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'openPack' )).toBe ( true );

    // 'makeRare' only valid on two commons.
    expect ( inventory.methodIsValid ( 'makeRare', pack0 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'makeRare', common0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'makeRare', common1 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'makeRare', rare0 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'makeRare', rare1 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'makeRare', ultraRare0 )).toBe ( false );

    // 'makeUltraRare' only valid on two rares.
    expect ( inventory.methodIsValid ( 'makeUltraRare', pack0 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'makeUltraRare', common0 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'makeUltraRare', common1 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'makeUltraRare', rare0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'makeUltraRare', rare1 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'makeUltraRare', ultraRare0 )).toBe ( false );

    // 'openPack' only valid on packs.
    expect ( inventory.methodIsValid ( 'openPack', pack0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'openPack', common0 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'openPack', common1 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'openPack', rare0 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'openPack', rare1 )).toBe ( false );
    expect ( inventory.methodIsValid ( 'openPack', ultraRare0 )).toBe ( false );
});

//----------------------------------------------------------------//
test ( 'test operators', () => {

    let schemaTemplate = schemaBuilder ( 'TEST_SCHEMA', 'schema.lua' )

        //----------------------------------------------------------------//
        .definition ( 'testAsset' )
            .field ( 'numberField', 123 )
            .field ( 'stringField', 'oneTwoThree' )
            .field ( 'keywordField', 'one two three' )

        //----------------------------------------------------------------//
        .method ( 'assetType', 1, 2, '' )
            .assetArg ( 'arg0', op.ASSET_TYPE ( 'testAsset' ))

        .method ( 'keyword', 1, 2, '' )
            .assetArg ( 'arg0', op.KEYWORD ( op.FIELD ( 'keywordField' ), 'two' ))

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

    let inventory = new Inventory ( schemaTemplate );

    const testAsset0 = inventory.addTestAsset ( 'testAsset' );

    inventory.process ();

    // test binding analysis

    // all of the schema methods should be valid for this inventory.
    expect ( inventory.methodIsValid ( 'assetType' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericEqual' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'stringEqual' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericNotEqual' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'stringNotEqual' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericGreater' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericGreaterOrEqual' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericLess' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericLessOrEqual' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalAnd' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalNotAnd' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalOr' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalNot' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalXor' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalNotXor' )).toBe ( true );

    expect ( inventory.methodIsValid ( 'add' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'div' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'mod' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'mul' )).toBe ( true );
    expect ( inventory.methodIsValid ( 'sub' )).toBe ( true );

    expect ( inventory.methodIsValid ( 'assetType', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericEqual', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'stringEqual', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericNotEqual', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'stringNotEqual', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericGreater', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericGreaterOrEqual', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericLess', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'numericLessOrEqual', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalAnd', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalNotAnd', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalOr', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalNot', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalXor', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'logicalNotXor', testAsset0 )).toBe ( true );

    expect ( inventory.methodIsValid ( 'add', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'div', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'mod', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'mul', testAsset0 )).toBe ( true );
    expect ( inventory.methodIsValid ( 'sub', testAsset0 )).toBe ( true );
});
