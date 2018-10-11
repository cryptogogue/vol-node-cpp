/* eslint-disable no-whitespace-before-property */

//----------------------------------------------------------------//
export const request = ( url, method, data ) => {

    method = (typeof method === 'undefined') ? 'GET' : method.toUpperCase ();

    return new Promise((resolve, reject) => {
        (() => {
            if (method === 'GET') {
                return fetch ( url );
            }
            else {
                return fetch ( url, {
                    method : method,
                    headers : { 'content-type': 'text/plain' },
                    body : JSON.stringify ( data )
                });
            }
        })()
        .then (( res ) => {
            console.log ( res );
            if ( !res.ok ) {
                reject ( new Error ( res.status ));
            }
            resolve ( res );
        })
        .catch (( error ) => {
            reject ( error.res );
        });
    });
}