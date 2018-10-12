import React from 'react';
import ReactDOM from 'react-dom';
import { createStore, applyMiddleware, compose } from 'redux';
import { Provider } from 'react-redux';
import createSagaMiddleware from 'redux-saga';

import MainFrame from './views/main_frame';
import { FileSystem } from './lib';
import reduce from './states/reducers';
import saga from './states/actions';


const fileSystem = new FileSystem('http://localhost:9000');

const composeEnhancers = window.__REDUX_DEVTOOLS_EXTENSION_COMPOSE__ || compose;
const sagaMiddleware = createSagaMiddleware();
const store = createStore(reduce, composeEnhancers(applyMiddleware(sagaMiddleware)));

sagaMiddleware.run(saga, {
  fileSystem,
});

ReactDOM.render(
  <Provider store={store}>
    <MainFrame connectSocket={({ onMessage, onClose, onError }) => {
      const connection = fileSystem.change();
      connection.addEventListener('message', onMessage);
      connection.addEventListener('close', onClose);
      connection.addEventListener('error', onError);
      return connection;
    }} />
  </Provider>,
  document.querySelector('#main-window'));
