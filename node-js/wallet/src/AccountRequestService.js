/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { NODE_TYPE, NODE_STATUS }   from './AppStateService';
import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, storage, util } from 'fgc';
import _                            from 'lodash';
import { action, computed, extendObservable, observe, observable } from 'mobx';

//================================================================//
// NodeInfoService
//================================================================//
export class AccountRequestService {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        
        this.revocable = new RevocableContext ();

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
                    const data = await this.revocable.fetchJSON ( appState.node + '/keys/' + keyID );

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

            this.revocable.timeout (() => { this.checkPendingRequests ( appState, delay )}, delay );
        }
        _fetch ();
    }

    //----------------------------------------------------------------//
    finalize () {

        this.revocable.finalize ();
    }
}
