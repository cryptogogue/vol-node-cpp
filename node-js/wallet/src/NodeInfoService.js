/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { NODE_TYPE, NODE_STATUS }   from './AppStateService';
import { assert, excel, Service, SingleColumnContainerView, storage, useService, util } from 'fgc';
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

        this.discoverNodes ( 5000 );
    }

    //----------------------------------------------------------------//
    discoverNodes ( delay ) {

        const discoverNode = async ( url ) => {

            try {
                await NodeInfoService.update ( this, this.appState, url )
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

    //----------------------------------------------------------------//
    static async update ( service, appState, url ) {

        const data = await service.revocableFetchJSON ( url );

        let { type } = appState.getNodeInfo ( url );

        if ( data.type === 'VOL_MINING_NODE' ) {
            type = NODE_TYPE.MINING;
        }
        if ( data.type === 'VOL_PROVIDER' ) {
            type = NODE_TYPE.MARKET;
        }

        appState.setNodeInfo ( url, type, NODE_STATUS.ONLINE, data.identity );
    }
}
