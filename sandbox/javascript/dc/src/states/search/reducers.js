import {
  SEARCH_NAME_TRY,
  SEARCH_NAME_SUCCEED,
} from './actions';


const initialState = {
  matched: [],
  history: [],
};


export default function reduceSearch (state = initialState, { type, payload }) {
  switch (type) {
    case SEARCH_NAME_TRY: {
      const { name } = payload;
      const history = state.history.filter(e => e !== name);
      history.unshift(name);
      return Object.assign({}, state, {
        history,
      });
    }
    case SEARCH_NAME_SUCCEED: {
      const { pathList } = payload;
      return Object.assign({}, state, {
        matched: pathList,
      });
    }
    default:
      return state;
  }
}
