import { all } from 'redux-saga/effects';

import { sagaGetList, sagaGetRootList, sagaGetFile } from './file_system/actions';


export default function * ({ fileSystem }) {
  yield all([
    sagaGetList(fileSystem),
    sagaGetRootList(fileSystem),
    sagaGetFile(fileSystem),
  ]);
}
