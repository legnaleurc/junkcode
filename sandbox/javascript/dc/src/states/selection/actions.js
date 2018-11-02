import { call, put, takeEvery, select } from 'redux-saga/effects';


export const SELECT_TOGGLE = 'SELECT_TOGGLE';
export const SELECT_MOVE_TRY = 'SELECT_MOVE_TRY';
export const SELECT_MOVE_SUCCEED = 'SELECT_MOVE_SUCCEED';
export const SELECT_MOVE_FAILED = 'SELECT_MOVE_FAILED';
export const SELECT_CONTINUOUSLY_TRY = 'SELECT_CONTINUOUSLY_TRY';
export const SELECT_CONTINUOUSLY_SUCCEED = 'SELECT_CONTINUOUSLY_SUCCEED';


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
    yield call(() => fileSystem.sync());
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
