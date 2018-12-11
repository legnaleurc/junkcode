import { call, put, takeEvery } from 'redux-saga/effects';


export const FS_LIST_GET_TRY = 'FS_LIST_GET_TRY';
export const FS_LIST_GET_SUCCEED = 'FS_LIST_GET_SUCCEED';
export const FS_LIST_GET_FAILED = 'FS_LIST_GET_FAILED';
export const FS_ROOT_GET_TRY = 'FS_ROOT_GET_TRY';
export const FS_ROOT_GET_SUCCEED = 'FS_ROOT_GET_SUCCEED';
export const FS_ROOT_GET_FAILED = 'FS_ROOT_GET_FAILED';
export const FS_STREAM_URL = 'FS_STREAM_URL';
export const FS_SYNC_TRY = 'FS_SYNC_TRY';
export const FS_SYNC_SUCCEED = 'FS_SYNC_SUCCEED';
export const FS_SYNC_FAILED = 'FS_SYNC_FAILED';
export const FS_SET_SORT = 'FS_SET_SORT';


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
    type: FS_SYNC_TRY,
  };
}


function postSyncSucceed (changeList) {
  return {
    type: FS_SYNC_SUCCEED,
    payload: {
      changeList,
    },
  };
}


function postSyncFailed (message) {
  return {
    type: FS_SYNC_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaPostSync (fileSystem) {
  yield takeEvery(FS_SYNC_TRY, function * () {
    try {
      const changeList = yield call(() => fileSystem.sync());
      yield put(postSyncSucceed(changeList));
    } catch (e) {
      yield put(postSyncFailed(e.message));
    }
  });
}


export function setSortFunction (key) {
  return {
    type: FS_SET_SORT,
    payload: {
      key,
    },
  };
}
