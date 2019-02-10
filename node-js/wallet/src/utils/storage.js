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

    return JSON.parse ( localStorage.getItem ( k ))
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
        console.log ( "inLocalStorage", v );
    }
    catch ( err ) {
        console.log ( "Write to local storage failed" );
    }
}
