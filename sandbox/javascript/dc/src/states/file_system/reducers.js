import {
  ROOT_LIST_GET_SUCCEED,
  LIST_GET_SUCCEED,
} from './actions';


const initialState = {
  nodes: {},
  roots: [],
};


function createNode (node) {
  return {
    id: node.id,
    name: node.name,
    children: node.is_folder ? [] : null,
    fetched: false,
  };
}


export default function reduceFileSystem (state = initialState, { type, payload }) {
  switch (type) {
    case ROOT_LIST_GET_SUCCEED: {
      const { nodes, roots } = state;
      for (const node of payload.children) {
        roots.push(node.id);
        nodes[node.id] = createNode(node);
      }
      return {
        nodes: Object.assign({}, nodes),
        roots: [...roots],
      };
    }
    case LIST_GET_SUCCEED: {
      const { nodes, roots } = state;
      const { id, children } = payload;
      for (const node of children) {
        nodes[node.id] = createNode(node);
      }
      const parent = nodes[id];
      parent.fetched = true;
      parent.children = children.map(node => node.id);
      return {
        nodes: Object.assign({}, nodes),
        roots,
      };
    }
    default:
      return state;
  }
}
