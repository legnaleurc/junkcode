import { all } from 'redux-saga/effects';

import {
  sagaGetList,
  sagaGetRootList,
  sagaGetStreamUrl,
  sagaPostSync,
} from './file_system/actions';
import {
  sagaMoveSelectedNodesTo,
  sagaContinuouslySelect,
} from './selection/actions';


export default function * ({ fileSystem }) {
  yield all([
    sagaGetList(fileSystem),
    sagaGetRootList(fileSystem),
    sagaGetStreamUrl(fileSystem),
    sagaPostSync(fileSystem),
    sagaMoveSelectedNodesTo(fileSystem),
    sagaContinuouslySelect(),
  ]);
}
