const fs            = require ( 'fs' );
const spawn			= require ( 'child_process' ).spawn;
const util			= require ( 'util' );

//const FILTER = 'WEB.CHAIN:';
const FILTER = '';

const queue = [];

//----------------------------------------------------------------//
function log ( prefix, data, filePath ) {

    let lines = data.toString ().split ( '\n' );
    let len = lines.length - 1;
    for ( let i = 0; i < len; ++i ) {

        let line = lines [ i ];
        if ( !FILTER || ( line.indexOf ( FILTER ) > -1 )) {
            if ( filePath ) {
                queue.push ({
                    path: filePath,
                    line: prefix + line,
                });
            }
            // else {
            //     console.log ( 'OOPS' );
            //     console.log ( prefix + line );
            // }
        }
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
	   logToFile ( logFileName, data.toString ());
	});

    child.stderr.on ( 'data', function ( data ) {
        logToFile ( logFileName, data.toString ());
    });
}

spawnWithArgs ( '9090: ', '../bin/server', '-m 9090 -p 9090 -g genesis-nokeys.in', './volume-logio/log/9090.log' );
spawnWithArgs ( '9091: ', '../bin/server', '-m 9091 -p 9091 -g genesis-nokeys.in', './volume-logio/log/9091.log' );
spawnWithArgs ( '9092: ', '../bin/server', '-m 9092 -p 9092 -g genesis-nokeys.in', './volume-logio/log/9092.log' );
spawnWithArgs ( '9093: ', '../bin/server', '-m 9093 -p 9093 -g genesis-nokeys.in', './volume-logio/log/9093.log' );
spawnWithArgs ( '9094: ', '../bin/server', '-m 9094 -p 9094 -g genesis-nokeys.in', './volume-logio/log/9094.log' );

//----------------------------------------------------------------//
async function mainLoop () {

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

mainLoop ();
