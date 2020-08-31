const fs            = require ( 'fs' );
const spawn			= require ( 'child_process' ).spawn;
const util			= require ( 'util' );

//const FILTER = 'WEB.CHAIN:';
const FILTER = '';

let loggingEnabled = false;
const queue = [];

//----------------------------------------------------------------//
function log ( prefix, data, filePath ) {

    if ( filePath ) {
        loggingEnabled = true;
        logToFile ( filePath, data.toString ());
        return;
    }
    return;

    let lines = data.toString ().split ( '\n' );
    let len = lines.length - 1;
    for ( let i = 0; i < len; ++i ) {

        let line = lines [ i ];
        if ( !FILTER || ( line.indexOf ( FILTER ) > -1 )) {
            console.log ( prefix + line );
        }
    }
}

//----------------------------------------------------------------//
async function loggingLoop () {

    function sleep ( ms ) {
        return new Promise ( resolve => setTimeout ( resolve, ms ));
    }

    while ( true ) {
        while ( queue.length ) {
            const entry = queue.shift ();
            fs.appendFileSync ( entry.path, entry.line );
        }
        await sleep ( 1 );
    }
}

//----------------------------------------------------------------//
function logToFile ( filePath, line ) {

    if ( filePath ) {
        queue.push ({
            path: filePath,
            line: line,
        });
    }         
}

//----------------------------------------------------------------//
function spawnWithArgs ( prefix, command, args, logFileName ) {

    console.log ( 'spawn with args' );
    
	let child = spawn ( command, args.split ( ' ' ));

	child.stdout.on ( 'data', function ( data ) {
	   log ( prefix, data, logFileName );
	});

    child.stderr.on ( 'data', function ( data ) {
        log ( prefix, data, logFileName );
    });
}

//================================================================//
// main
//================================================================//

const scenario = JSON.parse ( fs.readFileSync ( 'scenario.json' ));
for ( let miner of scenario.miners ) {

    const port = miner.port;

    spawnWithArgs (
        `${ port }: `,
        '../bin/server',
        `-m ${ port } -p ${ port } -g genesis`,
        miner.log ? `./volume-logio/log/${ port }.log` : false
    );
}

if ( loggingEnabled ) {
    loggingLoop ();
}
