import { all, call, put, takeEvery } from 'redux-saga/effects';

import {
  getRootListSucceed,
  getRootListFailed,
  ROOT_LIST_GET_TRY,
  getListSucceed,
  getListFailed,
  LIST_GET_TRY,
} from './actions';


function * getRootList ({ payload }) {
  const { fileSystem } = payload;
  try {
    const id = yield call(() => fileSystem.root());
    const children = yield call(() => fileSystem.list(id));
    yield put(getRootListSucceed(children));
  } catch (e) {
    yield put(getRootListFailed(e.message));
  }
}


function * getList ({ payload }) {
  const { fileSystem, id } = payload;
  try {
    const children = yield call(() => fileSystem.list(id));
    yield put(getListSucceed(id, children));
  } catch (e) {
    yield put(getListFailed(e.message));
  }
}


export function * rootSaga () {
  yield all([
    takeEvery(ROOT_LIST_GET_TRY, getRootList),
    takeEvery(LIST_GET_TRY, getList),
  ]);
}
