/* eslint-disable no-whitespace-before-property */
/* eslint-disable no-loop-func */

import React                from 'react';
import { Grid, Header }     from 'semantic-ui-react';

//================================================================//
// SingleColumnContainerView
//================================================================//
export const SingleColumnContainerView = ( props ) => {

    // The styles below are necessary for the correct render of this form.
    // You can do same with CSS, the main idea is that all the elements up to the `Grid`
    // below must have a height of 100%.

    return (

        <div style={{ margin:'5% 0 10px 0' }}>
            <style>{`
                body > div,
                body > div > div,
                body > div > div > div.login-form {
                    height: 100%;
                }
            `}</style>
            <Grid textAlign = "center" style = {{ height: '100%' }} verticalAlign = "middle">
                <Grid.Column style={{ maxWidth: 450 }}>
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
