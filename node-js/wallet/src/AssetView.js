/* eslint-disable no-whitespace-before-property */

import handlebars       from 'handlebars';
import { observer }     from 'mobx-react';
import React            from 'react';

const assetTemplate = handlebars.compile ( `

    <svg x = "0" y = "0" width = "2.5in" height = "3.5in" viewBox = "0 0 750 1050" preserveAspectRatio = "none">
        {{{ asset }}}
    </svg>
`);

//================================================================//
// AssetView
//================================================================//
const AssetView = observer (( props ) => {

    const { inventory, assetId } = props;
    
    let subProps = Object.assign ({}, props );
    delete subProps.inventory;
    delete subProps.assetId;

    const svg = assetTemplate ({ asset: inventory.assetLayouts [ assetId ]});

    return (
        <div { ...subProps } dangerouslySetInnerHTML = {{ __html: svg }}/>
    );
});

export default AssetView;