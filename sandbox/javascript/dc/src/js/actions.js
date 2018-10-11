export const LIST_GET_TRY = 'LIST_GET_TRY';
export const LIST_GET_SUCCEED = 'LIST_GET_SUCCEED';
export const LIST_GET_FAILED = 'LIST_GET_FAILED';
export const ROOT_LIST_GET_TRY = 'ROOT_LIST_GET_TRY';
export const ROOT_LIST_GET_SUCCEED = 'ROOT_LIST_GET_SUCCEED';
export const ROOT_LIST_GET_FAILED = 'ROOT_LIST_GET_FAILED';


export function getList (fileSystem, id) {
  return {
    type: LIST_GET_TRY,
    payload: {
      fileSystem,
      id,
    },
  };
}


export function getListSucceed (id, children) {
  return {
    type: LIST_GET_SUCCEED,
    payload: {
      id,
      children,
    },
  };
}


export function getListFailed (message) {
  return {
    type: LIST_GET_FAILED,
    payload: {
      message,
    },
  };
}


export function getRootList (fileSystem) {
  return {
    type: ROOT_LIST_GET_TRY,
    payload: {
      fileSystem,
    },
  };
}


export function getRootListSucceed (children) {
  return {
    type: ROOT_LIST_GET_SUCCEED,
    payload: {
      children,
    },
  };
}


export function getRootListFailed (message) {
  return {
    type: ROOT_LIST_GET_FAILED,
    payload: {
      message,
    },
  };
}
