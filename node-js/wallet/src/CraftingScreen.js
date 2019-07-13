/* eslint-disable no-whitespace-before-property */

import CraftingFormSelector     from './CraftingFormSelector';
import NavigationBar            from './NavigationBar';
import { useService }           from './stores/Service';
import { InventoryService }       from './stores/InventoryService';
import { observer }             from "mobx-react";
import React, { useContext }    from 'react';
import { Segment, Grid }        from 'semantic-ui-react';

//================================================================//
// CraftingScreen
//================================================================//
const CraftingScreen = observer (( props ) => {

    const accountId = props.match.params.accountId;

    // don't use context just now; needs a rewrite
    //const { appState } = useContext ( AppStateContext );

    const inventory = useService (() => new InventoryService ( accountId, 'http://localhost:9090' )); // TODO: get URL from context after rewrite

    return (
        <div>
            <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                <Grid.Column style = {{ maxWidth: 450 }}>

                    <NavigationBar navTitle = "Crafting" match = { props.match }/>

                    <Segment>
                        <CraftingFormSelector inventory = { inventory }/>
                    </Segment>

                </Grid.Column>
            </Grid>
        </div>
    );
});

export default CraftingScreen;
