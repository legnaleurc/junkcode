import {
  SELECT_TOGGLE,
  SELECT_CLEAR,
  SELECT_MOVE_SUCCEED,
  SELECT_SIBLING_LIST_SUCCEED,
  SELECT_MATCHED_LIST_SUCCEED,
  SELECT_DELETE_SUCCEED,
} from './actions';


const initialState = {
  table: {},
  last: null,
};


export default function reduceSelection (state = initialState, action) {
  switch (action.type) {
    case SELECT_TOGGLE: {
      const { table } = state;
      let { last } = state;
      const id = action.payload.id;
      if (table[id]) {
        delete table[id];
        last = null;
      } else {
        table[id] = true;
        last = id;
      }
      return Object.assign({}, state, {
        table: Object.assign({}, table),
        last,
      });
    }
    case SELECT_CLEAR:
      return Object.assign({}, state, {
        table: {},
        last: null,
      });
    case SELECT_MOVE_SUCCEED: {
      return Object.assign({}, state, {
        table: {},
        last: null,
      });
    }
    case SELECT_SIBLING_LIST_SUCCEED: {
      const { table, last } = state;
      const { list } = action.payload;
      for (const id of list) {
        table[id] = true;
      }
      return Object.assign({}, state, {
        table: Object.assign({}, table),
        last,
      });
    }
    case SELECT_MATCHED_LIST_SUCCEED: {
      const { table, last } = state;
      const { list } = action.payload;
      for (const id of list) {
        table[id] = true;
      }
      return Object.assign({}, state, {
        table: Object.assign({}, table),
        last,
      });
    }
    case SELECT_DELETE_SUCCEED: {
      return Object.assign({}, state, {
        table: {},
        last: null,
      });
    }
    default:
      return state;
  }
}
