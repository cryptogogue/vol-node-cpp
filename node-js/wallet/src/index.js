/* eslint-disable no-whitespace-before-property */

import './index.css';
import 'semantic-ui-css/semantic.min.css';

import { configure } from 'mobx';
configure ({
    enforceActions:     'always',
});

import { AccountRequestScreen }     from './AccountRequestScreen';
import { AccountScreen }            from './AccountScreen';
import { CraftingScreen }           from './CraftingScreen';
import { DebugAESScreen }           from './debug/DebugAESScreen';
import { DebugCryptoKeyScreen }     from './debug/DebugCryptoKeyScreen';
import { DebugDropzoneScreen }      from './debug/DebugDropzoneScreen';
import { DebugHandlebarsScreen }    from './debug/DebugHandlebarsScreen';
import { DebugInfiniteScrollScreen }  from './debug/DebugInfiniteScrollScreen';
import { DebugMobXScreen }          from './debug/DebugMobXScreen';
import { DebugPrintScreen }         from './debug/DebugPrintScreen';
import { DebugStripeScreen }        from './debug/DebugStripeScreen';
import { DebugTextFitterScreen }    from './debug/DebugTextFitterScreen';
import { ImportAccountScreen }      from './ImportAccountScreen';
import { InventoryScreen }          from './InventoryScreen';
import { LoginScreen }              from './LoginScreen';
import { RegisterScreen }           from './RegisterScreen';
import { ResetScreen }              from './ResetScreen';
import registerServiceWorker        from './util/registerServiceWorker';
import React                        from 'react';
import ReactDOM                     from 'react-dom';
import { BrowserRouter, Route, Link, Switch } from "react-router-dom";

// https://react-bootstrap.github.io/
// https://material-ui.com/
// https://www.primefaces.org/

//----------------------------------------------------------------//
const App = () => {

    return (<BrowserRouter>
        <div>
            <Switch>
                <Route exact path = "/:userID/accounts/import" component = { ImportAccountScreen }/>
                <Route exact path = "/accounts/import" component = { ImportAccountScreen }/>

                <Route exact path = "/:userID/accounts/new" component = { AccountRequestScreen }/>
                <Route exact path = "/accounts/new" component = { AccountRequestScreen }/>

                <Route exact path = "/:userID/accounts/:accountID" component = { AccountScreen }/>
                <Route exact path = "/accounts/:accountID" component = { AccountScreen }/>

                <Route exact path = "/:userID/accounts/:accountID/crafting/:methodName" component = { CraftingScreen }/>
                <Route exact path = "/accounts/:accountID/crafting/:methodName" component = { CraftingScreen }/>

                <Route exact path = "/:userID/accounts/:accountID/crafting" component = { CraftingScreen }/>
                <Route exact path = "/accounts/:accountID/crafting" component = { CraftingScreen }/>

                <Route exact path = "/:userID/accounts/:accountID/inventory" component = { InventoryScreen }/>
                <Route exact path = "/accounts/:accountID/inventory" component = { InventoryScreen }/>

                <Route exact path = "/:userID/accounts/:accountID/reset" component = { ResetScreen }/>
                <Route exact path = "/accounts/:accountID/reset" component = { ResetScreen }/>

                <Route exact path = "/:userID/accounts" component = { AccountScreen }/>
                <Route exact path = "/accounts" component = { AccountScreen }/>
                
                <Route exact path = "/:userID/login" component = { LoginScreen }/>
                <Route exact path = "/login" component = { LoginScreen }/>

                <Route exact path = "/debug/aes" component = { DebugAESScreen }/>
                <Route exact path = "/debug/cryptokey" component = { DebugCryptoKeyScreen }/>
                <Route exact path = "/debug/dropzone" component = { DebugDropzoneScreen }/>
                <Route exact path = "/debug/handlebars" component = { DebugHandlebarsScreen }/>
                <Route exact path = "/debug/infinitescroll" component = { DebugInfiniteScrollScreen }/>
                <Route exact path = "/debug/mobx" component = { DebugMobXScreen }/>
                <Route exact path = "/debug/print" component = { DebugPrintScreen }/>
                <Route exact path = "/debug/stripe" component = { DebugStripeScreen }/>
                <Route exact path = "/debug/textfitter" component = { DebugTextFitterScreen }/>

                <Route exact path = "/:userID/" component = { RegisterScreen }/>
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
