/* eslint-disable no-whitespace-before-property */

import { action, computed, extendObservable, observable, observe, runInAction } from 'mobx';
import { observer }                         from 'mobx-react';

//================================================================//
// TransactionFormFieldController
//================================================================//
export class TransactionFormFieldController {

    //----------------------------------------------------------------//
    constructor ( fieldName, friendlyName, defaultValue, initialValue ) {

        this.fieldName      = fieldName;
        this.friendlyName   = friendlyName;
        this.isHidden       = false;
        this.defaultValue   = defaultValue,

        extendObservable ( this, {
            error:          false,
            inputString:    initialValue === undefined ? '' : String ( initialValue ),
        });
    }

    //----------------------------------------------------------------//
    @computed
    get isComplete () {
        return ( this.inputString || !this.isRequired);
    }

    //----------------------------------------------------------------//
    @computed
    get isRequired () {
        return ( this.defaultValue === undefined );
    }

    //----------------------------------------------------------------//
    @computed
    get value () {

        if ( this.inputString ) {
            return this.coerce ? this.coerce ( this.inputString ) : this.inputString;
        }
        return this.defaultValue;
    }

    //----------------------------------------------------------------//
    @action
    setInputString ( inputString ) {

        this.inputString = String ( inputString ) || '';
        console.log ( 'SET INPUT STRING', this.fieldName, inputString, this.inputString );
    }
}

//================================================================//
// TransactionFormFieldController_AccountKey
//================================================================//
export class TransactionFormFieldController_AccountKey extends TransactionFormFieldController {

    //----------------------------------------------------------------//
    constructor ( fieldName, friendlyName, value ) {
        super ( fieldName, friendlyName, value );
    }
}

//================================================================//
// TransactionFormConstFieldController
//================================================================//
export class TransactionFormConstFieldController extends TransactionFormFieldController {

    //----------------------------------------------------------------//
    constructor ( fieldName, friendlyName, value ) {
        super ( fieldName, friendlyName, value );
    }
}

//================================================================//
// TransactionFormCryptoKeyFieldController
//================================================================//
export class TransactionFormCryptoKeyFieldController extends TransactionFormFieldController {

    //----------------------------------------------------------------//
    constructor ( fieldName, friendlyName, rows, defaultValue, initialValue ) {
        super ( fieldName, friendlyName, defaultValue, initialValue );
        this.rows = rows;
    }
}

//================================================================//
// TransactionFormIntegerFieldController
//================================================================//
export class TransactionFormIntegerFieldController extends TransactionFormFieldController {

    //----------------------------------------------------------------//
    constructor ( fieldName, friendlyName, defaultValue, initialValue ) {
        super ( fieldName, friendlyName, defaultValue, initialValue );
        this.coerce = ( input ) => { return Number ( input )};
    }
}

//================================================================//
// TransactionFormStringFieldController
//================================================================//
export class TransactionFormStringFieldController extends TransactionFormFieldController {

    //----------------------------------------------------------------//
    constructor ( fieldName, friendlyName, defaultValue, initialValue ) {
        super ( fieldName, friendlyName, defaultValue, initialValue );
    }
}

//================================================================//
// TransactionFormTextFieldController
//================================================================//
export class TransactionFormTextFieldController extends TransactionFormFieldController {

    //----------------------------------------------------------------//
    constructor ( fieldName, friendlyName, rows, defaultValue, initialValue ) {
        super ( fieldName, friendlyName, defaultValue, initialValue );
        this.rows = rows;
    }
}

export const FIELD_CLASS = {
    ACCOUNT_KEY:        TransactionFormFieldController_AccountKey,
    CONST:              TransactionFormConstFieldController,
    CRYPTO_KEY:         TransactionFormCryptoKeyFieldController,
    INTEGER:            TransactionFormIntegerFieldController,
    STRING:             TransactionFormStringFieldController,
    TEXT:               TransactionFormTextFieldController,
}
