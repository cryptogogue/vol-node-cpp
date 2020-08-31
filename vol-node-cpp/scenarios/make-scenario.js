const fs            = require ( 'fs' );
const util			= require ( 'util' );

const BASE_PORT     = 9090;

const miners = [];
const accounts = [];

const scenario = {
    miners: miners,
};

const genesis = {
    height:     0,
    time:       '1970-01-01T00:00:00UTC',
    transactions: [
        {
            bodyIn: {
                type:       'GENESIS',
                identity:   'LOCAL_SCENARIO_NET',
                accounts:   accounts,
            }
        },
    ],
};

//----------------------------------------------------------------//
for ( let i = 0; i < 16; ++i ) {

    const port = BASE_PORT + i;

    miners.push ({
        port:   port,
        log:    false,
    });

    accounts.push ({
        name: `${ port }`,
        grant: 10000,
        url: `http://127.0.0.1:${ port }/`,
    });
}

fs.writeFileSync ( 'scenario.json', JSON.stringify ( scenario, null, 4 ));
fs.writeFileSync ( 'genesis', JSON.stringify ( genesis, null, 4 ));
