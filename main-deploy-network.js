const spawn			= require ( 'child_process' ).spawn;
const util			= require ( 'util' );

//----------------------------------------------------------------//
function spawnWithArgs ( prefix, command, args ) {

	let child = spawn ( command, args.split ( ' ' ));

	child.stdout.on ( 'data', function ( data ) {

		let lines = data.toString ().split ( '\n' );
		let len = lines.length - 1;
		for ( let i = 0; i < len; ++i ) {
    		console.log ( prefix + lines [ i ]);
    	}
	});
}

//spawnWithArgs ( '9090: ', 'bin/server', '-p 9090 -k pkey0.pem -g genesis.signed' );
spawnWithArgs ( '9091: ', 'bin/server', '-p 9091 -k pkey0.pem -g genesis.signed' );
