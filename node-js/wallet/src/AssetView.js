/* eslint-disable no-whitespace-before-property */

import { observer }     from 'mobx-react';
import React            from 'react';

//================================================================//
// AssetView
//================================================================//
const AssetView = observer (( props ) => {

    const { inventory, assetId } = props;
    
    let subProps = Object.assign ({}, props );
    delete subProps.inventory;
    delete subProps.assetId;

    return (
        // <img src = { 'data:image/svg+xml,' + inventory.assetLayouts [ assetId ] }/>
        <div { ...subProps } dangerouslySetInnerHTML = {{ __html: inventory.assetLayouts [ assetId ]}}/>
    );
});

export default AssetView;