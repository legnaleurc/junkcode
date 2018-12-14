import { call, put, takeEvery, select } from 'redux-saga/effects';


const SEARCH_NAME_PRE = 'SEARCH_NAME_PRE';
export const SEARCH_NAME_TRY = 'SEARCH_NAME_TRY';
export const SEARCH_NAME_SUCCEED = 'SEARCH_NAME_SUCCEED';
export const SEARCH_NAME_FAILED = 'SEARCH_NAME_FAILED';


export function getSearchName (name) {
  return {
    type: SEARCH_NAME_PRE,
    payload: {
      name,
    },
  };
}


function getSearchNameTry (name) {
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
  yield takeEvery(SEARCH_NAME_PRE, function * ({ payload }) {
    try {
      const { loading } = yield select(state => state.search);
      if (loading) {
        return;
      }

      const { name } = payload;
      yield put(getSearchNameTry(name));

      const pathList = yield call(() => fileSystem.searchByName(name));
      yield put(getSearchNameSucceed(pathList));
    } catch (e) {
      yield put(getSearchNameFailed(e.message));
    }
  });
}
