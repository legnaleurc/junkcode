import { call, put, takeEvery } from 'redux-saga/effects';


export const LIST_GET_TRY = 'LIST_GET_TRY';
export const LIST_GET_SUCCEED = 'LIST_GET_SUCCEED';
export const LIST_GET_FAILED = 'LIST_GET_FAILED';
export const ROOT_LIST_GET_TRY = 'ROOT_LIST_GET_TRY';
export const ROOT_LIST_GET_SUCCEED = 'ROOT_LIST_GET_SUCCEED';
export const ROOT_LIST_GET_FAILED = 'ROOT_LIST_GET_FAILED';
export const FS_STREAM_URL = 'FS_STREAM_URL';
export const SYNC_POST = 'SYNC_POST';
export const NODE_UPSERT = 'NODE_UPSERT';


export function getList (id) {
  return {
    type: LIST_GET_TRY,
    payload: {
      id,
    },
  };
}


function getListSucceed (id, children) {
  return {
    type: LIST_GET_SUCCEED,
    payload: {
      id,
      children,
    },
  };
}


function getListFailed (message) {
  return {
    type: LIST_GET_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaGetList (fileSystem) {
  yield takeEvery(LIST_GET_TRY, function * ({ payload }) {
    const { id } = payload;
    try {
      const children = yield call(() => fileSystem.list(id));
      yield put(getListSucceed(id, children));
    } catch (e) {
      yield put(getListFailed(e.message));
    }
  });
}


export function getRootList () {
  return {
    type: ROOT_LIST_GET_TRY,
  };
}


function getRootListSucceed (children) {
  return {
    type: ROOT_LIST_GET_SUCCEED,
    payload: {
      children,
    },
  };
}


function getRootListFailed (message) {
  return {
    type: ROOT_LIST_GET_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaGetRootList (fileSystem) {
  yield takeEvery(ROOT_LIST_GET_TRY, function * () {
    try {
      const id = yield call(() => fileSystem.root());
      const children = yield call(() => fileSystem.list(id));
      yield put(getRootListSucceed(children));
    } catch (e) {
      yield put(getRootListFailed(e.message));
    }
  });
}


export function getStreamUrl (id, done) {
  return {
    type: FS_STREAM_URL,
    payload: {
      id,
      done,
    },
  };
}


export function * sagaGetStreamUrl (fileSystem) {
  yield takeEvery(FS_STREAM_URL, function * ({ payload }) {
    const { id, done } = payload;
    const url = yield call(() => fileSystem.stream(id));
    yield call(() => done(url));
  });
}


export function postSync () {
  return {
    type: SYNC_POST,
  };
}


export function * sagaPostSync (fileSystem) {
  yield takeEvery(SYNC_POST, function * () {
    yield call(() => fileSystem.sync());
  });
}


export function upsertNode (change) {
  return {
    type: NODE_UPSERT,
    payload: {
      change,
    },
  };
}
