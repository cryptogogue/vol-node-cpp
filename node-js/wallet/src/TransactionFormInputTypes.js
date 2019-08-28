/* eslint-disable no-whitespace-before-property */

//----------------------------------------------------------------//
export function integerField ( fieldName, friendlyName ) {
    return {
        fieldType:      'INTEGER',
        name:           fieldName,
        friendlyName:   friendlyName,
    };
}

//----------------------------------------------------------------//
export function stringField ( fieldName, friendlyName ) {
    return {
        fieldType:      'STRING',
        name:           fieldName,
        friendlyName:   friendlyName,
    };
}

//----------------------------------------------------------------//
export function textField ( fieldName, friendlyName, rows ) {
    return {
        fieldType:      'TEXT',
        name:           fieldName,
        friendlyName:   friendlyName,
        rows:           rows,
    };
}
