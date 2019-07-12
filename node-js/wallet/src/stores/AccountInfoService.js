/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import * as storage     from '../utils/storage';
import { Service }      from './Service';
import { action, computed, extendObservable, observe, observable } from 'mobx';

//================================================================//
// AppStateStore
//================================================================//
export class AccountInfoService extends Service {

    //----------------------------------------------------------------//
    constructor ( appState ) {
        super ();

        extendObservable ( this, {
            appState:   appState,
        });

        observe ( appState, 'accountId', ( change ) => {
            this.revokeAll ();
            this.syncAccountBalance ( 1000 );
        });
        this.syncAccountBalance ( 1000 );
    }

    //----------------------------------------------------------------//
    @action
    syncAccountBalance ( delay ) {

        if ( this.appState.accountId.length === 0 ) return;

        let updateBalance = async () => {

            const data = await this.revocableFetchJSON ( this.appState.node + '/accounts/' + this.appState.accountId );

            if ( data.account && ( data.account.accountName === this.appState.accountId )) {
                this.appState.setAccountInfo ( data.account.balance, data.account.nonce );
            }
        }
        this.revocablePromiseWithBackoff (() => updateBalance (), delay, 2, true );
    }
}
