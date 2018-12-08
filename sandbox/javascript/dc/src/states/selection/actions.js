import { call, put, takeEvery, select } from 'redux-saga/effects';

import { postSync } from '../file_system/actions';


export const SELECT_TOGGLE = 'SELECT_TOGGLE';
export const SELECT_MOVE_TRY = 'SELECT_MOVE_TRY';
export const SELECT_MOVE_SUCCEED = 'SELECT_MOVE_SUCCEED';
export const SELECT_MOVE_FAILED = 'SELECT_MOVE_FAILED';
export const SELECT_SIBLING_LIST_TRY = 'SELECT_SIBLING_LIST_TRY';
export const SELECT_SIBLING_LIST_SUCCEED = 'SELECT_SIBLING_LIST_SUCCEED';
export const SELECT_SIBLING_LIST_FAILED = 'SELECT_SIBLING_LIST_FAILED';
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
      const { table } = yield select(getLocalState);
      const srcList = Object.keys(table);
      yield call(() => fileSystem.move(srcList, id));
      yield put(moveSelectedNodesToSucceed());
    } catch (e) {
      yield put(moveSelectedNodesToFailed(e.message));
    }
    yield put(postSync());
  });
}


export function selectSiblingList (id) {
  return {
    type: SELECT_SIBLING_LIST_TRY,
    payload: {
      id,
    },
  };
}


function selectSiblingListSucceed (list) {
  return {
    type: SELECT_SIBLING_LIST_SUCCEED,
    payload: {
      list,
    },
  };
}


function selectSiblingListFailed (message) {
  return {
    type: SELECT_SIBLING_LIST_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaSelectSiblingList () {
  yield takeEvery(SELECT_SIBLING_LIST_TRY, function * ({ payload }) {
    const { id } = payload;
    const { last } = yield select(getLocalState);
    if (!last) {
      yield put(selectSiblingListFailed('NO_LATEST_SELECTION'));
      return;
    }
    const { nodes } = yield select(state => state.fileSystem);
    const node = nodes[last];
    if (!node.parentId) {
      yield put(selectSiblingListFailed('NO_SIBLING'));
      return;
    }
    const parent = nodes[node.parentId];
    if (!parent) {
      yield put(selectSiblingListFailed('NO_PARENT'));
      return;
    }
    const toIndex = parent.children.indexOf(id);
    if (toIndex < 0) {
      yield put(selectSiblingListFailed('NO_SUCH_SIBLING'));
      return;
    }
    const fromIndex = parent.children.indexOf(last);
    if (toIndex < fromIndex) {
      [fromIndex, toIndex] = [toIndex, fromIndex];
    }
    const list = parent.children.slice(fromIndex, toIndex + 1);
    yield put(selectSiblingListSucceed(list));
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
      const { table } = yield select(getLocalState);
      const srcList = Object.keys(table);
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


function viewSelectedNodeSucceed (imageList) {
  return {
    type: SELECT_COMIC_SUCCEED,
    payload: {
      imageList,
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
      const { table } = yield select(getLocalState);
      let srcList = Object.keys(table);
      if (srcList.length !== 1) {
        yield put(viewSelectedNodeFailed('SELECTED_MULTIPLE_OR_NONE'));
        return;
      }
      const id = srcList[0];
      srcList = yield call(() => fileSystem.imageList(id));
      srcList = srcList.map((data, index) => {
        data.url = fileSystem.image(id, index);
        return data;
      });
      yield put(viewSelectedNodeSucceed(srcList));
    } catch (e) {
      yield put(viewSelectedNodeFailed(e.message));
    }
  });
}
