import {
  SELECT_TOGGLE,
  SELECT_MOVE_SUCCEED,
  SELECT_CONTINUOUSLY_SUCCEED,
  SELECT_DELETE_SUCCEED,
} from './actions';


const initialState = {
  selection: {},
  last: null,
};


export default function reduceSelection (state = initialState, action) {
  switch (action.type) {
    case SELECT_TOGGLE: {
      const { selection } = state;
      let { last } = state;
      const id = action.payload.id;
      if (selection[id]) {
        delete selection[id];
        last = null;
      } else {
        selection[id] = true;
        last = id;
      }
      return {
        selection: Object.assign({}, selection),
        last,
      };
    }
    case SELECT_MOVE_SUCCEED: {
      return {
        selection: {},
        last: null,
      };
    }
    case SELECT_CONTINUOUSLY_SUCCEED: {
      const { selection, last } = state;
      const { list } = action.payload;
      for (const id of list) {
        selection[id] = true;
      }
      return {
        selection: Object.assign({}, selection),
        last,
      };
    }
    case SELECT_DELETE_SUCCEED: {
      return {
        selection: {},
        last: null,
      };
    }
    default:
      return state;
  }
}
