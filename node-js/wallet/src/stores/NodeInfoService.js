/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import * as storage                 from '../utils/storage';
import { NODE_TYPE, NODE_STATUS }   from './AppStateService';
import { Service }                  from './Service';
import { action, computed, extendObservable, observe, observable } from 'mobx';

//================================================================//
// NodeInfoService
//================================================================//
export class NodeInfoService extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        this.pendingURLs = {};

        extendObservable ( this, {
            appState:   appState,
        });

        this.discoverNodes ( 1000 );
    }

    //----------------------------------------------------------------//
    discoverNodes ( delay ) {

        const discoverNode = async ( url ) => {

            let { type } = this.appState.getNodeInfo ( url );

            try {

                const data = await this.revocableFetchJSON ( url );

                if ( data.type === 'VOL_MINING_NODE' ) {
                    type = NODE_TYPE.MINING;
                }
                if ( data.type === 'VOL_PROVIDER' ) {
                    type = NODE_TYPE.MARKET;
                }

                this.appState.setNodeInfo ( url, type, NODE_STATUS.ONLINE );
                delete ( this.pendingURLs [ url ]);
            }
            catch ( error ) {

                this.appState.setNodeInfo ( url, type, NODE_STATUS.OFFLINE );
                throw error;
            }
        }

        for ( let url in this.appState.nodes ) {
            if ( !( url in this.pendingURLs )) {
                this.pendingURLs [ url ] = true;
                this.revocablePromiseWithBackoff (() => discoverNode ( url ), delay );
            }
        }
        this.revocableTimeout (() => { this.discoverNodes ( delay )}, delay );
    }
}
