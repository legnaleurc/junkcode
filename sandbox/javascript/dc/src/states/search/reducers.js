import {
  SEARCH_NAME_SUCCEED,
} from './actions';


const initialState = {
  matched: [],
};


export default function reduceSearch (state = initialState, { type, payload }) {
  switch (type) {
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
