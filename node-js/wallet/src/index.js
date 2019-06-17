/* eslint-disable no-whitespace-before-property */
import './index.css';
import 'semantic-ui-css/semantic.min.css';

import AccountScreen            from './AccountScreen';
import { AppStateProvider }     from './AppStateProvider';
import ImportAccountScreen      from './ImportAccountScreen';
import InventoryScreen          from './InventoryScreen';
import LoginScreen              from './LoginScreen';
import MobXInventoryScreen      from './MobXInventoryScreen';
import MobXTestScreen           from './MobXTestScreen';
import NewAccountScreen         from './NewAccountScreen';
import RegisterScreen           from './RegisterScreen';
import registerServiceWorker    from './utils/registerServiceWorker';
import React                    from 'react';
import ReactDOM                 from 'react-dom';
import { BrowserRouter, Route, Link, Switch } from "react-router-dom";
import StripeTestScreen         from './StripeTestScreen';

//----------------------------------------------------------------//
const App = () => (
    <BrowserRouter>
        <div>
            <ul>DEBUG MENU
                <li><Link to = "/">Register</Link></li>
                <li><Link to = "/login">Login</Link></li>
                <li><Link to = "/accounts">Accounts</Link></li>
                <li><Link to = "/stripe">Stripe</Link></li>
            </ul>
            <hr />
            <AppStateProvider>
                <Switch>
                    <Route exact path = "/:userId/accounts/import" component = { ImportAccountScreen }/>
                    <Route exact path = "/accounts/import" component = { ImportAccountScreen }/>

                    <Route exact path = "/:userId/accounts/new" component = { NewAccountScreen }/>
                    <Route exact path = "/accounts/new" component = { NewAccountScreen }/>

                    <Route exact path = "/:userId/accounts/:accountId" component = { AccountScreen }/>
                    <Route exact path = "/accounts/:accountId" component = { AccountScreen }/>

                    <Route exact path = "/:userId/accounts/:accountId/inventory" component = { InventoryScreen }/>
                    <Route exact path = "/accounts/:accountId/inventory" component = { InventoryScreen }/>

                    <Route exact path = "/:userId/accounts" component = { AccountScreen }/>
                    <Route exact path = "/accounts" component = { AccountScreen }/>
                    
                    <Route exact path = "/:userId/login" component = { LoginScreen }/>
                    <Route exact path = "/login" component = { LoginScreen }/>

                    <Route exact path = "/mobx" component = { MobXTestScreen }/>
                    <Route exact path = "/mobx/inventory" component = { MobXInventoryScreen }/>
                    <Route exact path = "/stripe" component = { StripeTestScreen }/>

                    <Route exact path = "/:userId/" component = { RegisterScreen }/>
                    <Route exact path = "/" component = { RegisterScreen }/>
                </Switch>
            </AppStateProvider>
        </div>
    </BrowserRouter>
)

//----------------------------------------------------------------//
ReactDOM.render (
    <App/>,
    document.getElementById ( 'root' )
);

registerServiceWorker ();
