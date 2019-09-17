/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import * as storage     from './util/storage';
import { Service }      from './Service';
import { action, computed, extendObservable, observe, observable } from 'mobx';

//================================================================//
// AppStateService
//================================================================//
export class AccountInfoService extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        extendObservable ( this, {
            appState:   appState,
        });

        observe ( appState, 'accountID', ( change ) => {
            this.revokeAll ();
            this.syncAccountInfo ( 5000 );
        });
        this.syncAccountInfo ( 5000 );
    }

    //----------------------------------------------------------------//
    @action
    syncAccountInfo ( delay ) {

        if ( this.appState.accountID.length === 0 ) return;

        let updateInfo = async () => {

            try {

                const accountID = this.appState.accountID;
                const data = await this.revocableFetchJSON ( this.appState.node + '/accounts/' + accountID );

                const account = data.account;
                const entitlements = data.entitlements;

                if ( account ) {

                    this.appState.setAccountInfo ( account.balance, account.nonce );
                    this.appState.updateAccount ( account, entitlements );
                    this.appState.confirmTransactions ( account.nonce );

                    if ( account.name !== accountID ) {
                        this.appState.renameAccount ( accountID, account.name );
                    }
                }
            }
            catch ( error ) {
                this.appState.setAccountInfo ();
                throw error;
            }
        }
        this.revocablePromiseWithBackoff (() => updateInfo (), delay, true );
    }
}
