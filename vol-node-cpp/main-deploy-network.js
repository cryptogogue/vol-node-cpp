const spawn			= require ( 'child_process' ).spawn;
const util			= require ( 'util' );

function log ( prefix, data ) {

    let lines = data.toString ().split ( '\n' );
    let len = lines.length - 1;
    for ( let i = 0; i < len; ++i ) {

        let line = lines [ i ];
        if ( line.indexOf ( 'WEB.CHAIN:' ) > -1 ) {
            console.log ( prefix + line );
        }
    }
}

//----------------------------------------------------------------//
function spawnWithArgs ( prefix, command, args ) {

	let child = spawn ( command, args.split ( ' ' ));

	//child.stdout.on ( 'data', function ( data ) {
	//	log ( prefix, data );
	//});

    child.stderr.on ( 'data', function ( data ) {
        log ( prefix, data );
    });
}

//spawnWithArgs ( '9090: ', 'bin/server', '-p 9090 -k keys/pkey0.priv.json -g genesis.signed' );
spawnWithArgs ( '9091: ', 'bin/server', '-p 9091 -k keys/pkey1.priv.json -g genesis.signed' );
spawnWithArgs ( '9092: ', 'bin/server', '-p 9092 -k keys/pkey2.priv.json -g genesis.signed' );
spawnWithArgs ( '9093: ', 'bin/server', '-p 9093 -k keys/pkey3.priv.json -g genesis.signed' );
spawnWithArgs ( '9094: ', 'bin/server', '-p 9094 -k keys/pkey4.priv.json -g genesis.signed' );
