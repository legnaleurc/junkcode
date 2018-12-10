import { call, put, takeEvery } from 'redux-saga/effects';


export const FS_LIST_GET_TRY = 'FS_LIST_GET_TRY';
export const FS_LIST_GET_SUCCEED = 'FS_LIST_GET_SUCCEED';
export const FS_LIST_GET_FAILED = 'FS_LIST_GET_FAILED';
export const FS_ROOT_GET_TRY = 'FS_ROOT_GET_TRY';
export const FS_ROOT_GET_SUCCEED = 'FS_ROOT_GET_SUCCEED';
export const FS_ROOT_GET_FAILED = 'FS_ROOT_GET_FAILED';
export const FS_STREAM_URL = 'FS_STREAM_URL';
export const SYNC_POST_TRY = 'SYNC_POST_TRY';
export const SYNC_POST_SUCCEED = 'SYNC_POST_SUCCEED';
export const SYNC_POST_FAILED = 'SYNC_POST_FAILED';


export function getList (id) {
  return {
    type: FS_LIST_GET_TRY,
    payload: {
      id,
    },
  };
}


function getListSucceed (id, children) {
  return {
    type: FS_LIST_GET_SUCCEED,
    payload: {
      id,
      children,
    },
  };
}


function getListFailed (message) {
  return {
    type: FS_LIST_GET_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaGetList (fileSystem) {
  yield takeEvery(FS_LIST_GET_TRY, function * ({ payload }) {
    const { id } = payload;
    try {
      const children = yield call(() => fileSystem.list(id));
      yield put(getListSucceed(id, children));
    } catch (e) {
      yield put(getListFailed(e.message));
    }
  });
}


export function getRoot () {
  return {
    type: FS_ROOT_GET_TRY,
  };
}


function getRootSucceed (node, children) {
  return {
    type: FS_ROOT_GET_SUCCEED,
    payload: {
      node,
      children,
    },
  };
}


function getRootFailed (message) {
  return {
    type: FS_ROOT_GET_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaGetRoot (fileSystem) {
  yield takeEvery(FS_ROOT_GET_TRY, function * () {
    try {
      const node = yield call(() => fileSystem.root());
      const children = yield call(() => fileSystem.list(node.id));
      yield put(getRootSucceed(node, children));
    } catch (e) {
      yield put(getRootFailed(e.message));
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
    type: SYNC_POST_TRY,
  };
}


function postSyncSucceed (changeList) {
  return {
    type: SYNC_POST_SUCCEED,
    payload: {
      changeList,
    },
  };
}


function postSyncFailed (message) {
  return {
    type: SYNC_POST_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaPostSync (fileSystem) {
  yield takeEvery(SYNC_POST_TRY, function * () {
    try {
      const changeList = yield call(() => fileSystem.sync());
      yield put(postSyncSucceed(changeList));
    } catch (e) {
      yield put(postSyncFailed(e.message));
    }
  });
}
