import {
  SEARCH_NAME_TRY,
  SEARCH_NAME_SUCCEED,
  SEARCH_NAME_FAILED,
} from './actions';


const initialState = {
  loading: false,
  matched: [],
  history: [],
};


export default function reduceSearch (state = initialState, { type, payload }) {
  switch (type) {
    case SEARCH_NAME_TRY: {
      if (state.loading) {
        return state;
      }
      const { name } = payload;
      const history = state.history.filter(e => e !== name);
      history.unshift(name);
      return Object.assign({}, state, {
        loading: true,
        matched: [],
        history,
      });
    }
    case SEARCH_NAME_SUCCEED: {
      const { pathList } = payload;
      return Object.assign({}, state, {
        loading: false,
        matched: pathList,
      });
    }
    case SEARCH_NAME_FAILED: {
      return Object.assign({}, state, {
        loading: false,
        matched: [],
      });
    }
    default:
      return state;
  }
}
