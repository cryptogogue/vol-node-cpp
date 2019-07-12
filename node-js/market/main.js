/* eslint-disable no-whitespace-before-property */

process.on ( 'uncaughtException', function ( err ) {
    console.log ( err );
    process.exit ( 1 );
});

const PORT                  = process.env.PORT;
const MAKER_NAME            = process.env.MAKER_NAME;
const MAKER_KEY_NAME        = process.env.MAKER_KEY_NAME;
const MINER_URL             = process.env.MINER_URL;

if ( !PORT ) throw 'Missing PORT environment variable.';
if ( !MAKER_NAME ) throw 'Missing MAKER_NAME environment variable.';
if ( !MAKER_KEY_NAME ) throw 'Missing MAKER_KEY_NAME environment variable.';
if ( !MINER_URL ) throw 'Missing MINER_URL environment variable.';

const bodyParser    = require ( 'body-parser' );
const express       = require ( 'express' );
const fetch         = require ( 'node-fetch' );

let stripe;

const STRIPE_PUBLIC_KEY     = process.env.STRIPE_PUBLIC_KEY;
const STRIPE_SECRET_KEY     = process.env.STRIPE_SECRET_KEY;

if ( STRIPE_PUBLIC_KEY && STRIPE_SECRET_KEY ) {
    console.log ( 'LOADING STRIPE' );
    stripe = require ( 'stripe' )( STRIPE_SECRET_KEY );
}
else {
    if ( !STRIPE_PUBLIC_KEY ) console.log ( 'WARNING: Missing STRIPE_PUBLIC_KEY environment variable.' );
    if ( !STRIPE_SECRET_KEY ) console.log ( 'WARNING: Missing STRIPE_SECRET_KEY environment variable.' );
}

server = express ();

server.use ( function ( req, res, next ) {
    res.header ( 'Access-Control-Allow-Origin', '*' );
    res.header ( 'Access-Control-Allow-Headers', 'Origin, X-Requested-With, Content-Type, Accept' );
    next ();
});

server.use ( bodyParser.json ());
server.use ( bodyParser.urlencoded ({ extended: true }));

let router = express.Router ();

router.get      ( '/bid',       _getBid );
router.post     ( '/bid',       _postBid );
router.post     ( '/stripe',    _postCharge );
router.get      ( '/',          _getStatus );

server.use ( '/', router );

server.listen ( PORT, () => {
    console.log ( 'VOLITION STORE LISTENING ON PORT:', PORT );
})

//----------------------------------------------------------------//
function _fetchNonce () {

    return new Promise (( resolve, reject ) => {

        console.log ( 'FETCH NONCE' );

        fetch ( MINER_URL + '/accounts/' + MAKER_NAME )
        .then (( response ) => { return response.json (); })
        .then (( data ) => {
            
            if ( data.account ) {
                resolve ( data.account.nonce )
            }
            else {
                reject ( new Error ( 'problem getting nonce' ));
            }
        })
        .catch (( error ) => {
            reject ( error );
        });
    });
}

//----------------------------------------------------------------//
function _fetchStatus () {

    return new Promise (( resolve, reject ) => {

        console.log ( 'FETCH STATUS' );

        fetch ( MINER_URL + '/' )
        .then (( response ) => { return response.json (); })
        .then (( data ) => {
            resolve ( data )
        })
        .catch (( error ) => {
            reject ( error );
        });
    });
}

//----------------------------------------------------------------//
async function _getBid ( request, result ) {

    const message = {
        type: 'VOL_BID',
        volPrice: 0,
        maxSale: 10000,
        accountPrice: 0,
    };

    result.json ( message );
}

//----------------------------------------------------------------//
async function _getStatus ( request, result ) {

    // console.log ( 'MINER URL:', MINER_URL );

    // const status = await _fetchStatus ();
    // result.json ( status );

    const message = {
        type: 'VOL_PROVIDER',
    };

    result.json ( message );
}

//----------------------------------------------------------------//
async function _postBid ( request, result ) {

    try {

        const bid = request.body;

        let accountName     = bid.accountName;
        let keyName         = bid.keyName;
        let publicKey       = bid.publicKey;

        console.log ( 'BID: ',
            bid.accountName,
            bid.keyName,
            bid.publicKey,
            bid.amount
        );

        if ( !bid.accountName ) throw 'Missing account name.';
        if ( !bid.keyName ) throw 'Missing key name.';
        if ( !bid.publicKey ) throw 'Missing public key.';

        nonce = await _fetchNonce ();

        let transaction = {
            
            'type':     'OPEN_ACCOUNT',

            'maker': {
                'accountName':  '9090',
                'keyName':      'master',
                'gratuity':     0,
                'nonce':        nonce,
            },

            'accountName':  bid.accountName,
            'amount':       bid.amount || 1000,
            'keyName':      bid.keyName,
            'key': {
                'groupName':    'secp256k1',
                'type':         'EC_HEX',
                'publicKey':    bid.publicKey,   
            },
        };

        fetch ( MINER_URL + '/transactions', {
            method : 'POST',
            headers : { 'content-type': 'application/json' },
            body : JSON.stringify ( transaction )
        });

        result.json ( transaction );
    }
    catch ( error ) {

        console.log ( error );

        result.status ( 500 );
        result.send ( 'SOMETHING HAPPEN' );
    }
}

//----------------------------------------------------------------//
async function _postCharge ( request, result ) {

    console.log ( 'STRIPE TOKEN:', request.body.tokenID );

    try {

        let { status } = await stripe.charges.create ({
            amount: 2000,
            currency: 'usd',
            description: 'An example charge',
            source: request.body.tokenID
        });

        console.log ( 'STRIPE:', status );

        result.json ({ status });
    }
    catch ( error ) {
        console.log ( 'ERROR:', error );
        result.status ( 500 ).end ();
    }
}

