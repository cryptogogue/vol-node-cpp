/* eslint-disable no-whitespace-before-property */

// import { randomBytes }          from './randomBytes';
// import * as BigInteger          from 'bigi';
// import * as bip39               from 'bip39';
// import * as bitcoin             from 'bitcoinjs-lib';
// import keyutils                 from 'js-crypto-key-utils';
// import * as secp256k1           from 'secp256k1'

let makeSquap;

//================================================================//
// AbstractSquap
//================================================================//
class AbstractSquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        return false;
    }
}

//================================================================//
// AbstractBinarySquap
//================================================================//
class AbstractBinarySquap extends AbstractSquap {

    //----------------------------------------------------------------//
    constructor ( template ) {
        super ( template );
        this.left = makeSquap ( template.left );
        this.right = makeSquap ( template.right );
    }
}

//================================================================//
// AbstractFunctionSquap
//================================================================//
class AbstractFunctionSquap extends AbstractSquap {

    //----------------------------------------------------------------//
    constructor ( template ) {
        super ( template );
        this.args = [];

        for ( let i in template.args ) {
            this.args [ i ] = makeSquap ( template.args [ i ]);
        }
    }
}

//================================================================//
// AbstractUnarySquap
//================================================================//
class AbstractUnarySquap extends AbstractSquap {

    //----------------------------------------------------------------//
    constructor ( template ) {
        super ( template );
        this.param = makeSquap ( template.param );
    }
}

//================================================================//
// AddSquap
//================================================================//
class AddSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal + rightVal;
    }
}

//================================================================//
// AndSquap
//================================================================//
class AndSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal && rightVal;
    }
}

//================================================================//
// AssetTypeSquap
//================================================================//
class AssetTypeSquap extends AbstractFunctionSquap {

    //----------------------------------------------------------------//
    eval ( param ) {

        const className = this.args [ 0 ] && this.args [ 0 ].eval ()
        if ( className ) {
            return param.className === className;
        }
        return false;
    }
}

//================================================================//
// ConstSquap
//================================================================//
class ConstSquap extends AbstractSquap {

    //----------------------------------------------------------------//
    constructor ( template ) {
        super ( template );
        this.type = template.type;
        this.value = template.value;
    }

    //----------------------------------------------------------------//
    eval ( param ) {
        return this.value;
    }
}

//================================================================//
// DivSquap
//================================================================//
class DivSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal / rightVal;
    }
}

//================================================================//
// EqualSquap
//================================================================//
class EqualSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal === rightVal;
    }
}

//================================================================//
// FieldSquap
//================================================================//
class FieldSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        // TODO:
        return false;
    }
}

//================================================================//
// GreaterSquap
//================================================================//
class GreaterSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal > rightVal;
    }
}

//================================================================//
// GreaterOrEqualSquap
//================================================================//
class GreaterOrEqualSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal >= rightVal;
    }
}

//================================================================//
// InSquap
//================================================================//
class InSquap extends AbstractSquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        // TODO:
        return false;
    }
}

//================================================================//
// LessSquap
//================================================================//
class LessSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal < rightVal;
    }
}

//================================================================//
// LessOrEqualSquap
//================================================================//
class LessOrEqualSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal <= rightVal;
    }
}

//================================================================//
// ModSquap
//================================================================//
class ModSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal % rightVal;
    }
}

//================================================================//
// MulSquap
//================================================================//
class MulSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal * rightVal;
    }
}

//================================================================//
// NotSquap
//================================================================//
class NotSquap extends AbstractUnarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const val = this.param.eval ( param );
        return !val;
    }
}

//================================================================//
// NotEqualSquap
//================================================================//
class NotEqualSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal !== rightVal;
    }
}

//================================================================//
// OrSquap
//================================================================//
class OrSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal || rightVal;
    }
}

//================================================================//
// SubSquap
//================================================================//
class SubSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return leftVal - rightVal;
    }
}

//================================================================//
// XorSquap
//================================================================//
class XorSquap extends AbstractBinarySquap {

    //----------------------------------------------------------------//
    eval ( param ) {
        const leftVal = this.left.eval ( param );
        const rightVal = this.right.eval ( param );
        return !leftVal !== !rightVal;
    }
}

//----------------------------------------------------------------//
const factoryTable = {

    ADD:                ( template ) => new AddSquap ( template ),
    AND:                ( template ) => new AndSquap ( template ),
    ASSET_TYPE:         ( template ) => new AssetTypeSquap ( template ),
    CONST:              ( template ) => new ConstSquap ( template ),
    DIV:                ( template ) => new DivSquap ( template ),
    EQUAL:              ( template ) => new EqualSquap ( template ),
    FIELD:              ( template ) => new FieldSquap ( template ),
    GREATER:            ( template ) => new GreaterSquap ( template ),
    GREATER_OR_EQUAL:   ( template ) => new GreaterOrEqualSquap ( template ),
    IN:                 ( template ) => new InSquap ( template ),
    LESS:               ( template ) => new LessSquap ( template ),
    LESS_OR_EQUAL:      ( template ) => new LessOrEqualSquap ( template ),
    MOD:                ( template ) => new ModSquap ( template ),
    MUL:                ( template ) => new MulSquap ( template ),
    NOT:                ( template ) => new NotSquap ( template ),
    NOT_EQUAL:          ( template ) => new NotEqualSquap ( template ),
    OR:                 ( template ) => new OrSquap ( template ),
    SUB:                ( template ) => new SubSquap ( template ),
    XOR:                ( template ) => new XorSquap ( template ),
}

//----------------------------------------------------------------//
makeSquap = ( template ) => {

    return factoryTable [ template.op ]( template );
}

export { makeSquap };
