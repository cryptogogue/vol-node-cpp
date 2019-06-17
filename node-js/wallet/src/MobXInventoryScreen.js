/* eslint-disable no-whitespace-before-property */

import { useLocalStore }    from './stores/LocalStore';
import { InventoryStore }   from './stores/InventoryStore';
import { observer }         from "mobx-react";
import React                from 'react';
import { List }             from 'semantic-ui-react';

//================================================================//
// MobXInventoryScreen
//================================================================//
const MobXInventoryScreen = observer (( props ) => {

    console.log ( 'RENDER INVENTORY SCREEN' )

    const store = useLocalStore (() => new InventoryStore ());

    if ( store.loading === true ) {
        return (<div>{ 'LOADING' }</div>);
    }

    let assetListItems = [];
    for ( let assetID in store.inventory.assets ) {
        let asset = store.inventory.assets [ assetID ];
        const name = `${ assetID }: ${ asset.fields.displayName }`;
        assetListItems.push (<List.Item key = { assetID }>{ name }</List.Item>);
    }

    return (
        <div>
            <List>{ assetListItems }</List>
        </div>
    );
});

export default MobXInventoryScreen;