import { call, put, takeEvery } from 'redux-saga/effects';


export const SEARCH_NAME_TRY = 'SEARCH_NAME_TRY';
export const SEARCH_NAME_SUCCEED = 'SEARCH_NAME_SUCCEED';
export const SEARCH_NAME_FAILED = 'SEARCH_NAME_FAILED';


export function getSearchName (name) {
  return {
    type: SEARCH_NAME_TRY,
    payload: {
      name,
    },
  };
}


function getSearchNameSucceed (pathList) {
  return {
    type: SEARCH_NAME_SUCCEED,
    payload: {
      pathList,
    },
  };
}


function getSearchNameFailed (message) {
  return {
    type: SEARCH_NAME_FAILED,
    payload: {
      message,
    },
  };
}


export function * sagaGetSearchName (fileSystem) {
  yield takeEvery(SEARCH_NAME_TRY, function * ({ payload }) {
    try {
      const { name } = payload;
      const pathList = yield call(() => fileSystem.searchByName(name));
      yield put(getSearchNameSucceed(pathList));
    } catch (e) {
      yield put(getSearchNameFailed(e.message));
    }
  });
}
