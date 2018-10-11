import { all } from 'redux-saga/effects';

import { sagaGetList, sagaGetRootList } from './file_system/actions';


export default function * () {
  yield all([
    sagaGetList(),
    sagaGetRootList(),
  ]);
}
