/* eslint-disable no-whitespace-before-property */

import React, { Component }     from 'react';
import { Segment, Button }      from 'semantic-ui-react';

//================================================================//
// Transactions
//================================================================//
class Transactions extends Component {

    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );
        this.state = { transactions : "" };
    }

    //----------------------------------------------------------------//
    // Fetch account data from the blockchain
    getTransactions () {
        const URI = 'http://localhost:9090/blocks/';
        fetch ( URI )
        .then ( res => {
            if ( res.ok ) {
                console.log ( "res", res );
                return res.json ();
            }
            else {
                throw new Error ( 'Something went wrong' );
            }
        })
        .then ( data => {
            this.setState ({ transactions : data.blocks.cycles [ 0 ].blocks [ 0 ].transactions [ 0 ].type });
        })
        .catch ( error => this.setState ({ transactions : error.message }));
    }

    //----------------------------------------------------------------//
    render () {
        
        return (
            <div>
                <Segment>
                    <Button onClick = { this.getTransactions }>
                        Get Transactions
                    </Button>
                    {this.state.transactions && <p>Transaction 0: { this.state.transactions }</p>}
                </Segment>
            </div>
        )
    }
}

export default Transactions