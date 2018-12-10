import { call, put, takeEvery, select } from 'redux-saga/effects';

import { postSync } from '../file_system/actions';


export const SELECT_TOGGLE = 'SELECT_TOGGLE';
export const SELECT_CLEAR = 'SELECT_CLEAR';
export const SELECT_MOVE_TRY = 'SELECT_MOVE_TRY';
export const SELECT_MOVE_SUCCEED = 'SELECT_MOVE_SUCCEED';
export const SELECT_MOVE_FAILED = 'SELECT_MOVE_FAILED';
export const SELECT_SIBLING_LIST_TRY = 'SELECT_SIBLING_LIST_TRY';
export const SELECT_SIBLING_LIST_SUCCEED = 'SELECT_SIBLING_LIST_SUCCEED';
export const SELECT_SIBLING_LIST_FAILED = 'SELECT_SIBLING_LIST_FAILED';
export const SELECT_MATCHED_LIST_TRY = 'SELECT_MATCHED_LIST_TRY';
export const SELECT_MATCHED_LIST_SUCCEED = 'SELECT_MATCHED_LIST_SUCCEED';
export const SELECT_MATCHED_LIST_FAILED = 'SELECT_MATCHED_LIST_FAILED';
export const SELECT_DELETE_TRY = 'SELECT_DELETE_TRY';
export const SELECT_DELETE_SUCCEED = 'SELECT_DELETE_SUCCEED';
export const SELECT_DELETE_FAILED = 'SELECT_DELETE_FAILED';


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


export function clearSelection () {
  return {
    type: SELECT_CLEAR,
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


export function selectMatchedList (id) {
  return {
    type: SELECT_MATCHED_LIST_TRY,
    payload: {
      id,
    },
  };
}


function selectMatchedListSucceed (list) {
  return {
    type: SELECT_MATCHED_LIST_SUCCEED,
    payload: {
      list,
    },
  };
}


function selectMatchedListFailed (message) {
  return {
    type: SELECT_MATCHED_LIST_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaSelectMatchedList () {
  yield takeEvery(SELECT_MATCHED_LIST_TRY, function * ({ payload }) {
    const { last } = yield select(getLocalState);
    if (!last) {
      yield put(selectMatchedListFailed('NO_LATEST_SELECTION'));
      return;
    }
    const { matched } = yield select(state => state.search);
    if (!matched) {
      yield put(selectMatchedListFailed('NOTHING_MATCHED'));
      return;
    }
    const fromIndex = matched.findIndex(({ id }) => (id === last));
    if (fromIndex < 0) {
      yield put(selectMatchedListFailed('NO_SUCH_ID'));
      return;
    }
    const toIndex = matched.findIndex(({ id }) => (id === payload.id));
    if (toIndex < 0) {
      yield put(selectMatchedListFailed('NO_SUCH_ID'));
      return;
    }
    if (toIndex < fromIndex) {
      [fromIndex, toIndex] = [toIndex, fromIndex];
    }
    let list = matched.slice(fromIndex, toIndex + 1);
    list = list.map(({ id }) => id);
    yield put(selectMatchedListSucceed(list));
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
