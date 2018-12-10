import {
  MPV_LOAD_TRY,
  MPV_LOAD_SUCCEED,
} from './actions';


const initialState = {
  imageList: [],
};


export default function reduceMultiPage (state = initialState, { type, payload }) {
  switch (type) {
    case MPV_LOAD_TRY: {
      return Object.assign({}, state, {
        imageList: [],
      });
    }
    case MPV_LOAD_SUCCEED: {
      return Object.assign({}, state, {
        imageList: payload.imageList,
      });
    }
    default:
      return state;
  }
}
