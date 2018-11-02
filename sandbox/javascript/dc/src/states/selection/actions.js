import { call, put, takeEvery, select } from 'redux-saga/effects';


export const SELECT_TOGGLE = 'SELECT_TOGGLE';
export const SELECT_MOVE_TRY = 'SELECT_MOVE_TRY';
export const SELECT_MOVE_SUCCEED = 'SELECT_MOVE_SUCCEED';
export const SELECT_MOVE_FAILED = 'SELECT_MOVE_FAILED';


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
