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

    const store = useLocalStore (() => new InventoryStore ( '9090', 'http://localhost:9090' ));

    if ( store.loading === true ) {
        return (<div>{ 'LOADING' }</div>);
    }

    let assetListItems = [];
    for ( let assetID in store.assets ) {

        let methodListString = '';

        const methodBindingsForAssetID = store.getCraftingMethodBindingsForAssetID ( assetID );
        for ( let methodName in methodBindingsForAssetID ) {
            const binding = methodBindingsForAssetID [ methodName ];
            if ( binding.valid ) {
                methodListString = methodListString.length ? `${ methodName }, ${ methodListString }` : methodName;
            }
        }

        const asset = store.assets [ assetID ];
        const name = `${ assetID }: ${ asset.fields.displayName } [${ methodListString }]`;
        assetListItems.push (<List.Item key = { assetID }>{ name }</List.Item>);
    }

    let methodListItems = [];
    const methodBindings = store.getCraftingMethodBindings ();
    for ( let methodName in methodBindings ) {
        const binding = methodBindings [ methodName ];
        const name = `${ methodName }: ${ binding.valid ? 'OK' : 'nope' }`;
        methodListItems.push (<List.Item key = { methodName }>{ name }</List.Item>);
    }

    return (
        <div>
            <h3>Methods</h3>
            <List>{ methodListItems }</List>
            <h3>Assets</h3>
            <List>{ assetListItems }</List>
        </div>
    );
});

export default MobXInventoryScreen;