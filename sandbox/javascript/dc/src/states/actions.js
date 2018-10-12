import { all } from 'redux-saga/effects';

import { sagaGetList, sagaGetRootList } from './file_system/actions';


export default function * ({ fileSystem }) {
  yield all([
    sagaGetList(fileSystem),
    sagaGetRootList(fileSystem),
  ]);
}
