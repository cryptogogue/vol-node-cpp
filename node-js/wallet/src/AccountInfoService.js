/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import { assert, excel, hooks, RevocableContext, SingleColumnContainerView, storage, util } from 'fgc';
import { action, computed, extendObservable, observe, observable } from 'mobx';

//================================================================//
// AppStateService
//================================================================//
export class AccountInfoService {

    //----------------------------------------------------------------//
    constructor ( appState ) {

        this.revocable = new RevocableContext ();

        extendObservable ( this, {
            appState:   appState,
        });

        observe ( appState, 'accountID', ( change ) => {
            this.revocable.revokeAll ();
            this.syncAccountInfo ( 5000 );
        });
        this.syncAccountInfo ( 5000 );
    }

    //----------------------------------------------------------------//
    finalize () {

        this.revocable.finalize ();
    }

    //----------------------------------------------------------------//
    @action
    syncAccountInfo ( delay ) {

        if ( this.appState.accountID.length === 0 ) return;

        let updateInfo = async () => {

            try {
                await AccountInfoService.update ( this, this.appState );
            }
            catch ( error ) {
                this.appState.setAccountInfo ();
                throw error;
            }
        }
        this.revocable.promiseWithBackoff (() => updateInfo (), delay, true );
    }

    //----------------------------------------------------------------//
    static async update ( service, appState ) {

        const accountID = appState.accountID;
        const data = await service.revocable.fetchJSON ( appState.node + '/accounts/' + accountID );

        const account = data.account;
        const entitlements = data.entitlements;

        if ( account ) {

            appState.setAccountInfo ( account.balance, account.nonce );
            appState.updateAccount ( account, entitlements );
            appState.confirmTransactions ( account.nonce );

            if ( account.name !== accountID ) {
                appState.renameAccount ( accountID, account.name );
            }
        }
    }
}
