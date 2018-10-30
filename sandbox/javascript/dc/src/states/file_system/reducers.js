import {
  ROOT_LIST_GET_SUCCEED,
  LIST_GET_SUCCEED,
  NODE_UPSERT,
} from './actions';


const initialState = {
  nodes: {},
  roots: [],
};


function createNode (node) {
  return {
    id: node.id,
    name: node.name,
    parent_id: node.parent_id,
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
      nodes[id] = Object.assign({}, parent, {
        fetched: true,
        children: children.map(node => node.id),
      });
      return {
        nodes: Object.assign({}, nodes),
        roots,
      };
    }
    case NODE_UPSERT: {
      const { nodes, roots } = state;
      const { change } = payload;
      if (change.removed) {
        roots = roots.filter(id => id !== change.id);
        const node = nodes[change.id];
        let parent = nodes[node.parent_id];
        if (parent) {
          parent = Object.assign({}, parent, {
            children: parent.children.filter(id => id !== change.id),
          });
          nodes[node.parent_id] = parent;
        }
        delete nodes[change.id];
      } else {
        const node = createNode(change.node);
        // TODO update roots
        let parent = nodes[node.parent_id];
        if (parent && parent.fetched) {
          parent = Object.assign({}, parent, {
            children: [node.id, ...parent.children],
          });
          nodes[node.parent_id] = parent;
        }
        nodes[node.id] = node;
      }
      return {
        nodes: Object.assign({}, nodes),
        roots,
      };
    }
    default:
      return state;
  }
}
