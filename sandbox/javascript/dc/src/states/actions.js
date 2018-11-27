import { all } from 'redux-saga/effects';

import {
  sagaGetList,
  sagaGetRootList,
  sagaGetStreamUrl,
  sagaPostSync,
  sagaGetSearchName,
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
    sagaGetSearchName(fileSystem),
    sagaMoveSelectedNodesTo(fileSystem),
    sagaContinuouslySelect(),
  ]);
}
