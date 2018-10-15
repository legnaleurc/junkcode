import { SELECT_TOGGLE } from './actions';


const initialState = {
  selection: {},
};


export default function reduceSelection (state = initialState, action) {
  switch (action.type) {
    case SELECT_TOGGLE: {
      const { selection } = state;
      const id = action.payload.id;
      if (selection[id]) {
        delete selection[id];
      } else {
        selection[id] = true;
      }
      return {
        selection: Object.assign({}, selection),
      };
    }
    default:
      return state;
  }
}
