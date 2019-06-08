/* eslint-disable no-whitespace-before-property */

const assert    = require ( 'assert' );
const fs        = require ( 'fs' );

//----------------------------------------------------------------//
function makeBinaryOp ( opname ) {
    return ( left, right ) => {
        return {
            op:         opname,
            left:       wrapLiteral ( left ),
            right:      wrapLiteral ( right ),
        };
    };
}

//----------------------------------------------------------------//
function makeConstOp ( opname ) {
    return ( value ) => {

        let type = typeof ( value );

        return {
            op:         opname,
            type:       type.toUpperCase (),
            value:      value,
        };
    };
}

//----------------------------------------------------------------//
// function makeFuncOp ( opname ) {
//     return ( ...args ) => {

//         let cleanArgs = [];

//         args.forEach ( function ( arg ) {

//             let argType = typeof ( arg );

//             if ( argType != 'object' ) {
//                 arg = op.CONST ( arg );
//             }
//             cleanArgs.push ( arg );
//         });

//         return {
//             op:         opname,
//             args:       cleanArgs,
//         };
//     };
// }

//----------------------------------------------------------------//
function makeIndexOp ( opname ) {
    return ( arg0, arg1 ) => {

        // index args are always assumed to be literals for now.
        // can add support for operator args later. if we need it.

        if ( arg0 && arg1 ) {
            return {
                op:         opname,
                paramID:    arg0,
                value:      arg1,
            };
        }
        return {
            op:         opname,
            value:      arg0,
        };
    };
}

//----------------------------------------------------------------//
function makeUnaryOp ( opname ) {
    return ( param ) => {
        return {
            op:         opname,
            param:      wrapLiteral ( param ),
        };
    };
}

//----------------------------------------------------------------//
function wrapLiteral ( param ) {

    return ( typeof ( param ) === 'object' ) && param || op.CONST ( param );
}

//----------------------------------------------------------------//
const op = {
    ADD:                makeBinaryOp    ( 'ADD' ),
    AND:                makeBinaryOp    ( 'AND' ),
    ASSET_TYPE:         makeIndexOp     ( 'ASSET_TYPE' ),
    CONST:              makeConstOp     ( 'CONST' ),
    DIV:                makeBinaryOp    ( 'DIV' ),
    EQUAL:              makeBinaryOp    ( 'EQUAL' ),
    FIELD:              makeIndexOp     ( 'FIELD' ),
    GREATER:            makeBinaryOp    ( 'GREATER' ),
    GREATER_OR_EQUAL:   makeBinaryOp    ( 'GREATER_OR_EQUAL' ),
    KEYWORD:            makeBinaryOp    ( 'KEYWORD' ),
    LESS:               makeBinaryOp    ( 'LESS' ),
    LESS_OR_EQUAL:      makeBinaryOp    ( 'LESS_OR_EQUAL' ),
    MOD:                makeBinaryOp    ( 'MOD' ),
    MUL:                makeBinaryOp    ( 'MUL' ),
    NOT:                makeUnaryOp     ( 'NOT' ),
    NOT_EQUAL:          makeBinaryOp    ( 'NOT_EQUAL' ),
    OR:                 makeBinaryOp    ( 'OR' ),
    SUB:                makeBinaryOp    ( 'SUB' ),
    XOR:                makeBinaryOp    ( 'XOR' ),
}

const MUTABLE           = 'MUTABLE';
const NUMBER            = 'NUMBER';
const STRING            = 'STRING';

const TYPE_NUMERIC      = 'NUMERIC';
const TYPE_STRING       = 'STRING';

const MEDIA_AUDIO       = 'MEDIA_AUDIO';
const MEDIA_IMAGE       = 'MEDIA_IMAGE';
const MEDIA_TEXT        = 'MEDIA_TEXT';
const MEDIA_VIDEO       = 'MEDIA_VIDEO';

const SCHEMA_BUILDER_ADDING_SCHEMA                  = 'SCHEMA_BUILDER_ADDING_SCHEMA';
const SCHEMA_BUILDER_ADDING_ASSET_DEFINITION        = 'ADDING_ASSET_DEFINITION';
const SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE          = 'ADDING_ASSET_TEMPLATE';
const SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD    = 'ADDING_ASSET_TEMPLATE_FIELD';
const SCHEMA_BUILDER_ADDING_METHOD                  = 'ADDING_ASSET_METHOD';

//================================================================//
// SchemaBuilder
//================================================================//
class SchemaBuilder {

    //----------------------------------------------------------------//
    assetArg ( name, qualifier ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ));
        this.top ().assetArgs [ name ] = qualifier;
        return this;
    }

    //----------------------------------------------------------------//
    assetDefinition ( name, base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        this.push (
            SCHEMA_BUILDER_ADDING_ASSET_DEFINITION,
            {
                implements:     base,
                fields:         {},
            },
            ( schema, assetDefinition ) => {
                schema.assetDefinitions [ name ] = assetDefinition;
            }
        );
        return this;
    }

    //----------------------------------------------------------------//
    assetTemplate ( name ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        this.push (
            SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE,
            {
                fields:     {},
            },
            ( schema, assetTemplate ) => {
                schema.assetTemplates [ name ] = assetTemplate;
            }
        );
        return this;
    }

    //----------------------------------------------------------------//
    boolean ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD ));
        this.top ().type = 'BOOL';
        return this;
    }

    //----------------------------------------------------------------//
    constArg ( name, qualifier ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ));
        this.top ().constArgs [ name ] = qualifier;
        return this;
    }

    //----------------------------------------------------------------//
    constraint ( name, description, qualifier ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_METHOD ));
        this.top ().constraints.push ({
            description:    description,
            constraint:     qualifier,
        });
        return this;
    }

    //----------------------------------------------------------------//
    constructor ( name, lua ) {

        this.stack = [];

        this.schema = {
            name:   name,
            lua:    lua,
            assetTemplates:     {},
            assetDefinitions:   {},
            methods:            {},
        };

        this.push (
            SCHEMA_BUILDER_ADDING_SCHEMA,
            this.schema
        );
    }

    //----------------------------------------------------------------//
    done () {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));
        return this.schema;
    }

    //----------------------------------------------------------------//
    extends ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE ));
        this.top ().extends = base;
        return this;
    }

    //----------------------------------------------------------------//
    field ( name, value ) {

        if ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE )) {

            let field = {
                array:          false,
                mutable:        false,
            }

            this.push (
                SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD,
                field,
                ( assetTemplate, field ) => {
                    assert ( 'type' in field );
                    assetTemplate.fields [ name ] = field;
                }
            );
            return this;
        }

        if ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_DEFINITION )) {

            this.top ().fields [ name ] = value;
            return this;
        }

        assert ( false );
        return this;
    }

    //----------------------------------------------------------------//
    method ( name, weight, maturity, description ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_SCHEMA ));

        this.push (
            SCHEMA_BUILDER_ADDING_METHOD,
            {
                weight:         weight,
                maturity:       maturity,
                description:    description,
                assetArgs:      {},
                constArgs:      {},
                constraints:    [],
            },
            ( schema, method ) => {
                schema.methods [ name ] = method;
            }
        );
        return this;
    }

    //----------------------------------------------------------------//
    numeric ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD ));
        this.top ().type = TYPE_NUMERIC;
        return this;
    }

    //----------------------------------------------------------------//
    mutable ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD ));
        this.top ().mutable = true;
        return this;
    }

    //----------------------------------------------------------------//
    popTo ( state ) {

        let found = false;
        for ( let i in this.stack ) {
            if ( this.stack [ i ].state === state ) {
                found = true;
                break;
            }
        }
        if ( !found ) return false;

        let i = this.stack.length - 1;
        while ( this.stack [ i ].state !== state ) {
            this.stack [ i ].resolve ( this.stack [ i - 1 ].container, this.stack [ i ].container );
            this.stack.pop ();
            i--;
        }
        return true;
    }

    //----------------------------------------------------------------//
    push ( state, container, resolve ) {

        this.stack.push ({
            state:          state,
            container:      container,
            resolve:        resolve,
        });
    }

    //----------------------------------------------------------------//
    string ( base ) {

        assert ( this.popTo ( SCHEMA_BUILDER_ADDING_ASSET_TEMPLATE_FIELD ));
        this.top ().type = TYPE_STRING;
        return this;
    }

    //----------------------------------------------------------------//
    top () {
        return this.stack [ this.stack.length - 1 ].container;
    }
}

//export { SchemaBuilder };

var exports = module.exports = ( name, lua ) => new SchemaBuilder ( name, lua );
exports.op = op;
