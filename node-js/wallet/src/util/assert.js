/* eslint-disable no-whitespace-before-property */

//----------------------------------------------------------------//
export function assert ( condition, error ) {
    if ( !condition ) throw error || 'Assetion failed.'
}
