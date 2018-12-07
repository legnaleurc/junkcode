import { all } from 'redux-saga/effects';

import {
  sagaGetList,
  sagaGetRoot,
  sagaGetStreamUrl,
  sagaPostSync,
  sagaGetSearchName,
} from './file_system/actions';
import {
  sagaMoveSelectedNodesTo,
  sagaContinuouslySelect,
  sagaViewSelectedNode,
} from './selection/actions';


export default function * ({ fileSystem }) {
  yield all([
    sagaGetList(fileSystem),
    sagaGetRoot(fileSystem),
    sagaGetStreamUrl(fileSystem),
    sagaPostSync(fileSystem),
    sagaGetSearchName(fileSystem),
    sagaMoveSelectedNodesTo(fileSystem),
    sagaContinuouslySelect(),
    sagaViewSelectedNode(fileSystem),
  ]);
}
