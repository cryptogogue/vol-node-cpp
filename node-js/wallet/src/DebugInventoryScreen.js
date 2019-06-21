/* eslint-disable no-whitespace-before-property */

import { useLocalStore }    from './stores/LocalStore';
import { InventoryStore }   from './stores/InventoryStore';
import { observer }         from "mobx-react";
import React                from 'react';
import { List }             from 'semantic-ui-react';

//================================================================//
// DebugInventoryScreen
//================================================================//
const DebugInventoryScreen = observer (( props ) => {

    console.log ( 'RENDER INVENTORY SCREEN' )

    const store = useLocalStore (() => new InventoryStore ());

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

    let assetLayouts = [];
    for ( let assetID in store.assets ) {
        assetLayouts.push (<div key = { assetID } dangerouslySetInnerHTML = {{ __html: store.assetLayouts [ assetID ]}}/>);
    }

    return (
        <div>
            <h3>Methods</h3>
            <List>{ methodListItems }</List>
            <h3>Assets</h3>
            <List>{ assetListItems }</List>
            <h3>Asset Layouts</h3>
            { assetLayouts }
        </div>
    );
});

export default DebugInventoryScreen;