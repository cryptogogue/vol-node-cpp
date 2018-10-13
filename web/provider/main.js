/* eslint-disable no-whitespace-before-property */

process.on ( 'uncaughtException', function ( err ) {
    console.log ( err );
    process.exit ( 1 );
});

const bodyParser    = require ( 'body-parser' );
const dotenv        = require ( 'dotenv' );
const express       = require ( 'express' );
const fetch         = require ( 'node-fetch' );

dotenv.load ();

const PORT              = process.env.PORT;
const MAKER_NAME        = process.env.MAKER_NAME;
const MAKER_KEY_NAME    = process.env.MAKER_KEY_NAME;
const MINER_URL         = process.env.MINER_URL;

if ( !PORT ) throw 'Missing PORT environment variable.';
if ( !MAKER_NAME ) throw 'Missing MAKER_NAME environment variable.';
if ( !MAKER_KEY_NAME ) throw 'Missing MAKER_KEY_NAME environment variable.';
if ( !MINER_URL ) throw 'Missing MINER_URL environment variable.';

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

            if ( data.account && data.account.nonce ) {
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
            'amount':   bid.amount || 0,
            'keyName':  bid.keyName,
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

