import { call, put, takeEvery, select } from 'redux-saga/effects';

import { postSync } from '../file_system/actions';


export const SELECT_TOGGLE = 'SELECT_TOGGLE';
export const SELECT_MOVE_TRY = 'SELECT_MOVE_TRY';
export const SELECT_MOVE_SUCCEED = 'SELECT_MOVE_SUCCEED';
export const SELECT_MOVE_FAILED = 'SELECT_MOVE_FAILED';
export const SELECT_CONTINUOUSLY_TRY = 'SELECT_CONTINUOUSLY_TRY';
export const SELECT_CONTINUOUSLY_SUCCEED = 'SELECT_CONTINUOUSLY_SUCCEED';
export const SELECT_DELETE_TRY = 'SELECT_DELETE_TRY';
export const SELECT_DELETE_SUCCEED = 'SELECT_DELETE_SUCCEED';
export const SELECT_DELETE_FAILED = 'SELECT_DELETE_FAILED';
export const SELECT_COMIC_TRY = 'SELECT_COMIC_TRY';
export const SELECT_COMIC_SUCCEED = 'SELECT_COMIC_SUCCEED';
export const SELECT_COMIC_FAILED = 'SELECT_COMIC_FAILED';


function getLocalState (state) {
  return state.selection;
}


export function toggleSelection (id) {
  return {
    type: SELECT_TOGGLE,
    payload: {
      id,
    },
  };
}


export function moveSelectedNodesTo (id) {
  return {
    type: SELECT_MOVE_TRY,
    payload: {
      id,
    },
  };
}


function moveSelectedNodesToSucceed () {
  return {
    type: SELECT_MOVE_SUCCEED,
  };
}


function moveSelectedNodesToFailed (message) {
  return {
    type: SELECT_MOVE_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaMoveSelectedNodesTo (fileSystem) {
  yield takeEvery(SELECT_MOVE_TRY, function * ({ payload }) {
    const { id } = payload;
    try {
      const { selection } = yield select(getLocalState);
      const srcList = Object.keys(selection);
      yield call(() => fileSystem.move(srcList, id));
      yield put(moveSelectedNodesToSucceed());
    } catch (e) {
      yield put(moveSelectedNodesToFailed(e.message));
    }
    yield put(postSync());
  });
}


export function continuouslySelect (id) {
  return {
    type: SELECT_CONTINUOUSLY_TRY,
    payload: {
      id,
    },
  };
}


function continuouslySelectSucceed (list) {
  return {
    type: SELECT_CONTINUOUSLY_SUCCEED,
    payload: {
      list,
    },
  };
}


export function * sagaContinuouslySelect () {
  yield takeEvery(SELECT_CONTINUOUSLY_TRY, function * ({ payload }) {
    const { id } = payload;
    const { last } = yield select(getLocalState);
    if (!last) {
      // TODO fallback to toggleSelect?
      return;
    }
    const { nodes } = yield select(state => state.fileSystem);
    const node = nodes[last];
    const parent = nodes[node.parent_id];
    if (!parent) {
      // FIXME root problem
      return;
    }
    const toIndex = parent.children.indexOf(id);
    if (toIndex < 0) {
      // should not cross folder
      return;
    }
    const fromIndex = parent.children.indexOf(last);
    if (toIndex < fromIndex) {
      [fromIndex, toIndex] = [toIndex, fromIndex];
    }
    const list = parent.children.slice(fromIndex, toIndex + 1);
    yield put(continuouslySelectSucceed(list));
  });
}


export function deleteSelectedNodes () {
  return {
    type: SELECT_DELETE_TRY,
  };
}


function deleteSelectedNodesSucceed () {
  return {
    type: SELECT_DELETE_SUCCEED,
  };
}


function deleteSelectedNodesFailed (message) {
  return {
    type: SELECT_DELETE_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaDeleteSelectedNodes (fileSystem) {
  yield takeEvery(SELECT_DELETE_TRY, function * () {
    try {
      const { selection } = yield select(getLocalState);
      const srcList = Object.keys(selection);
      yield call(() => fileSystem.trash(srcList));
      yield put(deleteSelectedNodesSucceed());
    } catch (e) {
      yield put(deleteSelectedNodesFailed(e.message));
    }
    yield put(postSync());
  });
}


export function viewSelectedNode () {
  return {
    type: SELECT_COMIC_TRY,
  };
}


function viewSelectedNodeSucceed () {
  return {
    type: SELECT_COMIC_SUCCEED,
    payload: {
      manifest,
    },
  };
}


function viewSelectedNodeFailed (message) {
  return {
    type: SELECT_COMIC_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaViewSelectedNode (fileSystem) {
  yield takeEvery(SELECT_COMIC_TRY, function * () {
    try {
      const { selection } = yield select(getLocalState);
      let srcList = Object.keys(selection);
      if (srcList.length !== 1) {
        yield put(viewSelectedNodeFailed('SELECTED_MULTIPLE_OR_NONE'));
        return;
      }
      srcList = yield call(() => fileSystem.imageList(srcList[0]));
      yield put(viewSelectedNodeSucceed(srcList));
    } catch (e) {
      yield put(viewSelectedNodeFailed(e.message));
    }
  });
}
