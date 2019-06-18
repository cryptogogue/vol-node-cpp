/* eslint-disable no-whitespace-before-property */

import CraftingFormSelector     from './CraftingFormSelector';
import NavigationBar            from './NavigationBar';
import { useLocalStore }        from './stores/LocalStore';
import { InventoryStore }       from './stores/InventoryStore';
import { observer }             from "mobx-react";
import React                    from 'react';
import { Segment, Grid }        from 'semantic-ui-react';

//================================================================//
// CraftingScreen
//================================================================//
const CraftingScreen = observer (( props ) => {

    const accountId = props.match.params.accountId;
    //const { minerURLs } = this.props.appState;

    const inventory = useLocalStore (() => new InventoryStore ( accountId, 'http://localhost:9090' ));

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
