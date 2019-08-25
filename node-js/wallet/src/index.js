/* eslint-disable no-whitespace-before-property */

import './index.css';
import 'semantic-ui-css/semantic.min.css';

import { configure } from 'mobx';
configure ({
    enforceActions:     'always',
});

import AccountScreen            from './AccountScreen';
import CraftingScreen           from './CraftingScreen';
import DebugAESScreen           from './debug/DebugAESScreen';
import DebugCryptoKeyScreen     from './debug/DebugCryptoKeyScreen';
import DebugDropzoneScreen      from './debug/DebugDropzoneScreen';
import DebugHandlebarsScreen    from './debug/DebugHandlebarsScreen';
import DebugMobXScreen          from './debug/DebugMobXScreen';
import DebugPrintScreen         from './debug/DebugPrintScreen';
import DebugStripeScreen        from './debug/DebugStripeScreen';
import DebugTextFitterScreen    from './debug/DebugTextFitterScreen';
import ImportAccountScreen      from './ImportAccountScreen';
import InventoryScreen          from './InventoryScreen';
import LoginScreen              from './LoginScreen';
import NewAccountScreen         from './NewAccountScreen';
import RegisterScreen           from './RegisterScreen';
import registerServiceWorker    from './util/registerServiceWorker';
import React                    from 'react';
import ReactDOM                 from 'react-dom';
import { BrowserRouter, Route, Link, Switch } from "react-router-dom";

// https://react-bootstrap.github.io/
// https://material-ui.com/
// https://www.primefaces.org/

//----------------------------------------------------------------//
const App = () => {

    return (<BrowserRouter>
        <div>
            <Switch>
                <Route exact path = "/:userId/accounts/import" component = { ImportAccountScreen }/>
                <Route exact path = "/accounts/import" component = { ImportAccountScreen }/>

                <Route exact path = "/:userId/accounts/new" component = { NewAccountScreen }/>
                <Route exact path = "/accounts/new" component = { NewAccountScreen }/>

                <Route exact path = "/:userId/accounts/:accountId" component = { AccountScreen }/>
                <Route exact path = "/accounts/:accountId" component = { AccountScreen }/>

                <Route exact path = "/:userId/accounts/:accountId/crafting/:methodName" component = { CraftingScreen }/>
                <Route exact path = "/accounts/:accountId/crafting/:methodName" component = { CraftingScreen }/>

                <Route exact path = "/:userId/accounts/:accountId/crafting" component = { CraftingScreen }/>
                <Route exact path = "/accounts/:accountId/crafting" component = { CraftingScreen }/>

                <Route exact path = "/:userId/accounts/:accountId/inventory" component = { InventoryScreen }/>
                <Route exact path = "/accounts/:accountId/inventory" component = { InventoryScreen }/>

                <Route exact path = "/:userId/accounts" component = { AccountScreen }/>
                <Route exact path = "/accounts" component = { AccountScreen }/>
                
                <Route exact path = "/:userId/login" component = { LoginScreen }/>
                <Route exact path = "/login" component = { LoginScreen }/>

                <Route exact path = "/debug/aes" component = { DebugAESScreen }/>
                <Route exact path = "/debug/cryptokey" component = { DebugCryptoKeyScreen }/>
                <Route exact path = "/debug/dropzone" component = { DebugDropzoneScreen }/>
                <Route exact path = "/debug/handlebars" component = { DebugHandlebarsScreen }/>
                <Route exact path = "/debug/mobx" component = { DebugMobXScreen }/>
                <Route exact path = "/debug/print" component = { DebugPrintScreen }/>
                <Route exact path = "/debug/stripe" component = { DebugStripeScreen }/>
                <Route exact path = "/debug/textfitter" component = { DebugTextFitterScreen }/>

                <Route exact path = "/:userId/" component = { RegisterScreen }/>
                <Route exact path = "/" component = { RegisterScreen }/>
            </Switch>
        </div>
    </BrowserRouter>);
}

//----------------------------------------------------------------//
ReactDOM.render (
    <App/>,
    document.getElementById ( 'root' )
);

registerServiceWorker ();
