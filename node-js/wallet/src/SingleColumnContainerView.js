/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import React                from 'react';
import { Grid, Header }     from 'semantic-ui-react';

//================================================================//
// SingleColumnContainerView
//================================================================//
export const SingleColumnContainerView = ( props ) => {

    const style = props.width === 0 ? {} : { width: typeof ( props.width ) === 'number' ? props.width : 450 };

    return (
        <div style = {{ margin: '10px 0 10px 0' }}>
            <Grid
                textAlign = "center"
                verticalAlign = "middle"
            >
                <Grid.Column style={ style }>
                    <If condition = { props.title && props.title.length > 0 }>
                        <Header as="h2" color="teal" textAlign="center">
                            { props.title }
                        </Header>
                    </If>
                    { props.children }
                </Grid.Column>
            </Grid>
        </div>
    );
}
