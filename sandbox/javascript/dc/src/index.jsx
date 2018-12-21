import React from 'react';
import ReactDOM from 'react-dom';
import { createStore, applyMiddleware } from 'redux';
import { Provider } from 'react-redux';
import createSagaMiddleware from 'redux-saga';
import { composeWithDevTools } from 'redux-devtools-extension/developmentOnly';

import Application from './views/application';
import { FileSystem } from './lib';
import reduce from './states/reducers';
import saga from './states/actions';
import { getRoot } from './states/file_system/actions';

import './index.scss';


const fileSystem = new FileSystem('http://localhost:9000');

const sagaMiddleware = createSagaMiddleware();
const store = createStore(reduce, composeWithDevTools(applyMiddleware(sagaMiddleware)));

sagaMiddleware.run(saga, {
  fileSystem,
});

ReactDOM.render(
  <Provider store={store}>
    <Application />
  </Provider>,
  document.querySelector('body > .body'));

store.dispatch(getRoot());
