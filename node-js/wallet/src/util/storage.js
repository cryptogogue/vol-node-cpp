/* eslint-disable no-whitespace-before-property */

//----------------------------------------------------------------//
// Delete all data in local storage
export const clear = () => {
    
    try {
        localStorage.clear ();
        console.log ( "Data deleted" );
    }
    catch ( err ) {
        return undefined
    }
}

//----------------------------------------------------------------//
export const getItem = ( k ) => {

    const v = localStorage.getItem ( k );
    console.log ( "fromLocalStorage", k, v );
    return JSON.parse ( v )
}

//----------------------------------------------------------------//
export const removeItem = ( k ) => {

    localStorage.removeItem ( k );
}

//----------------------------------------------------------------//
export const setItem = ( k, v ) => {

    try {
        const serializedState = JSON.stringify ( v );
        localStorage.setItem ( k, serializedState );
        console.log ( "inLocalStorage", k, serializedState );
    }
    catch ( err ) {
        console.log ( "Write to local storage failed" );
    }
}
