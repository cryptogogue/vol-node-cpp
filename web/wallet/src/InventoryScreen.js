/* eslint-disable no-whitespace-before-property */

import { withAppStateAndUser }  from './AppStateProvider';
import BaseComponent            from './BaseComponent';
import React                    from 'react';
import { Dropdown, Segment, Header, Icon, Divider, Modal, Grid } from 'semantic-ui-react';

//================================================================//
// InventoryScreen
//================================================================//
class InventoryScreen extends BaseComponent {
    
    //----------------------------------------------------------------//
    constructor ( props ) {
        super ( props );

        this.state = {
            assets : [],
        };

        this.fetchInventory ();
    }

    //----------------------------------------------------------------//
    async fetchInventory () {

        const accountId = this.getAccountId ();
        const { minerURLs } = this.props.appState;

        if ( minerURLs.size === 0 ) {
            this.revocableTimeout (() => { this.fetchInventory ()}, 100 );
            return;
        }

        try {

            let url;

            // ugh
            minerURLs.forEach (( minerURL ) => {
                url = minerURL;
            });

            const data = await this.revocableFetchJSON ( url + '/accounts/' + accountId + '/inventory' );

            if ( data.inventory ) {

                this.setState ({ assets : data.inventory.assets });
            }
        }
        catch ( error ) {
            console.log ( error );
        }
    }

    //----------------------------------------------------------------//
    getAccountId () {
        let accountId = this.props.match.params && this.props.match.params.accountId;
        return accountId && ( accountId.length > 0 ) && accountId;
    }

    //----------------------------------------------------------------//
    render () {

        const { assets } = this.state;

        return (
            <div>
            { assets.map (( asset, idx ) => {
                return (<p key = { 'asset' + idx }>{ asset.className }</p>);
            })}

            </div>
        );
    }
}

export default withAppStateAndUser ( InventoryScreen );
