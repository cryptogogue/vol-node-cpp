/* eslint-disable no-whitespace-before-property */
import './index.css';
import 'semantic-ui-css/semantic.min.css';

import AccountScreen            from './AccountScreen';
import { AppStateProvider }     from './AppStateProvider';
import LoginScreen              from './LoginScreen';
import NewAccountScreen         from './NewAccountScreen';
import RegisterScreen           from './RegisterScreen';
import registerServiceWorker    from './utils/registerServiceWorker';
import React                    from 'react';
import ReactDOM                 from 'react-dom';
import { BrowserRouter, Route, Link, Switch } from "react-router-dom";

//----------------------------------------------------------------//
const App = () => (
    <BrowserRouter>
        <div>
            <ul>DEBUG MENU
                <li><Link to = "/">Register</Link></li>
                <li><Link to = "/login">Login</Link></li>
                <li><Link to = "/accounts">Accounts</Link></li>
            </ul>
            <hr />
            <AppStateProvider value = { this.state }>
                <Switch>
                    <Route exact path = "/accounts/new" component = { NewAccountScreen }/>
                    <Route path = "/accounts/:accountId" component = { AccountScreen }/>
                    <Route path = "/accounts" component = { AccountScreen }/>
                    <Route exact path = "/login" component = { LoginScreen }/>
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
