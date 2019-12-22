/* eslint-disable no-whitespace-before-property */

import './index.css';
import 'semantic-ui-css/semantic.min.css';

import * as cardmotron from 'cardmotron';
import * as fgc from 'fgc';

import { configure } from 'mobx';
configure ({
    enforceActions:     'always',
});

import { AccountRequestScreen }         from './AccountRequestScreen';
import { AccountScreen }                from './AccountScreen';
import { CraftingScreen }               from './CraftingScreen';
import { DebugStripeScreen }            from './debug/DebugStripeScreen';
import { ImportAccountScreen }          from './ImportAccountScreen';
import { InventoryScreen }              from './InventoryScreen';
import { LoginScreen }                  from './LoginScreen';
import { RegisterScreen }               from './RegisterScreen';
import { ResetScreen }                  from './ResetScreen';
import registerServiceWorker            from './util/registerServiceWorker';
import React                            from 'react';
import ReactDOM                         from 'react-dom';
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

                <Route exact path = "/debug/aes"                component = { fgc.debug.AESScreen }/>
                <Route exact path = "/debug/barcode/pdf417"     component = { fgc.debug.BarcodePDF417Screen }/>
                <Route exact path = "/debug/barcode/qr"         component = { fgc.debug.BarcodeQRScreen }/>
                <Route exact path = "/debug/cardmotron"         component = { cardmotron.EditorScreen }/>
                <Route exact path = "/debug/cryptokey"          component = { fgc.debug.CryptoKeyScreen }/>
                <Route exact path = "/debug/dropzone"           component = { fgc.debug.DropzoneScreen }/>
                <Route exact path = "/debug/filepicker"         component = { fgc.debug.FilePickerScreen }/>
                <Route exact path = "/debug/handlebars"         component = { fgc.debug.HandlebarsScreen }/>
                <Route exact path = "/debug/infinitescroll"     component = { fgc.debug.InfiniteScrollScreen }/>
                <Route exact path = "/debug/mobx"               component = { fgc.debug.MobXScreen }/>
                <Route exact path = "/debug/print"              component = { fgc.debug.PrintScreen }/>
                <Route exact path = "/debug/squap"              component = { cardmotron.debug.SquapScreen }/>
                <Route exact path = "/debug/stripe"             component = { DebugStripeScreen }/>
                <Route exact path = "/debug/textfitter"         component = { fgc.debug.TextFitterScreen }/>
                <Route exact path = "/debug/textstyle"          component = { fgc.debug.TextStyleScreen }/>

                <Route exact path = "/util/schema"              component = { cardmotron.SchemaScreen }/>

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
