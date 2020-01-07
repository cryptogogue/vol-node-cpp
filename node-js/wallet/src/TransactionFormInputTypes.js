/* eslint-disable no-whitespace-before-property */

//----------------------------------------------------------------//
export function cryptoField ( fieldName, friendlyName, rows, defaultValue ) {

    const isRequired = typeof ( defaultValue ) !== 'string';

    return {
        fieldType:          'CRYPTO',
        name:               fieldName,
        friendlyName:       friendlyName,
        rows:               rows,
        defaultValue:       isRequired ? null : defaultValue,
        isRequired:         isRequired,
    };
}

//----------------------------------------------------------------//
export function integerField ( fieldName, friendlyName, defaultValue ) {

    const isRequired = typeof ( defaultValue ) !== 'number';

    return {
        fieldType:          'INTEGER',
        name:               fieldName,
        friendlyName:       friendlyName,
        defaultValue:       isRequired ? null : defaultValue,
        isRequired:         isRequired,
    };
}

//----------------------------------------------------------------//
export function stringField ( fieldName, friendlyName, defaultValue ) {

    const isRequired = typeof ( defaultValue ) !== 'string';

    return {
        fieldType:          'STRING',
        name:               fieldName,
        friendlyName:       friendlyName,
        defaultValue:       isRequired ? null : defaultValue,
        isRequired:         isRequired,
    };
}

//----------------------------------------------------------------//
export function textField ( fieldName, friendlyName, rows, defaultValue ) {

    const isRequired = typeof ( defaultValue ) !== 'string';

    return {
        fieldType:          'TEXT',
        name:               fieldName,
        friendlyName:       friendlyName,
        rows:               rows,
        defaultValue:       isRequired ? null : defaultValue,
        isRequired:         isRequired,
    };
}
