import { all } from 'redux-saga/effects';

import {
  sagaGetList,
  sagaGetRoot,
  sagaGetStreamUrl,
  sagaPostSync,
} from './file_system/actions';
import { sagaGetSearchName } from './search/actions';
import {
  sagaMoveSelectedNodesTo,
  sagaSelectSiblingList,
  sagaSelectMatchedList,
  sagaDeleteSelectedNodes,
} from './selection/actions';
import { sagaLoadMultiPageViewer } from './multipage/actions';


export default function * ({ fileSystem }) {
  yield all([
    sagaGetList(fileSystem),
    sagaGetRoot(fileSystem),
    sagaGetStreamUrl(fileSystem),
    sagaPostSync(fileSystem),
    sagaGetSearchName(fileSystem),
    sagaMoveSelectedNodesTo(fileSystem),
    sagaSelectSiblingList(),
    sagaSelectMatchedList(),
    sagaDeleteSelectedNodes(fileSystem),
    sagaLoadMultiPageViewer(fileSystem),
  ]);
}
