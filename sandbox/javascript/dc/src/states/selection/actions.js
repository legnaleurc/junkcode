export const SELECT_TOGGLE = 'SELECT_TOGGLE';


export function toggleSelection (id) {
  return {
    type: SELECT_TOGGLE,
    payload: {
      id,
    },
  };
}
