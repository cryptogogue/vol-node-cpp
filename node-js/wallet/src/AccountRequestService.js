/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import * as storage                 from './util/storage';
import { NODE_TYPE, NODE_STATUS }   from './AppStateService';
import { Service }                  from './Service';
import _                            from 'lodash';
import { action, computed, extendObservable, observe, observable } from 'mobx';

//================================================================//
// NodeInfoService
//================================================================//
export class AccountRequestService extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        this.checkPendingRequests ( appState, 5000 );
    }

    //----------------------------------------------------------------//
    checkPendingRequests ( appState, delay ) {

        const _fetch = async () => {

            for ( let requestID in appState.pendingAccounts ) {

                const pendingAccount = appState.pendingAccounts [ requestID ];
                if ( pendingAccount.readyToImport ) continue;

                try {

                    const keyID = pendingAccount.keyID;
                    const data = await this.revocableFetchJSON ( appState.node + '/keys/' + keyID );

                    const keyInfo = data && data.keyInfo;

                    if ( keyInfo ) {
                        appState.updateAccountRequest (
                            requestID,
                            keyInfo.accountName, 
                            keyInfo.keyName
                        );
                    }
                }
                catch ( error ) {
                    console.log ( error );
                }
            }

            this.revocableTimeout (() => { this.checkPendingRequests ( appState, delay )}, delay );
        }
        _fetch ();
    }
}
