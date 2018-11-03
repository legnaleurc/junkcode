import {
  ROOT_LIST_GET_SUCCEED,
  LIST_GET_SUCCEED,
  SYNC_POST_SUCCEED,
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
    case SYNC_POST_SUCCEED: {
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
        const node = nodes[change.node.id];
        const newNode = createNode(change.node);
        if (node && node.parent_id !== newNode.parent_id) {
          // remove child from old parent
          let parent = nodes[node.parent_id];
          if (parent && parent.fetched) {
            parent = Object.assign({}, parent, {
              children: parent.children.filter(id => id !== node.id),
            });
            nodes[node.parent_id] = parent;
          }
          // insert to new parent
          parent = nodes[newNode.parent_id];
          if (parent && parent.fetched) {
            parent = Object.assign({}, parent, {
              children: [newNode.id, ...parent.children],
            });
            nodes[newNode.parent_id] = parent;
          }
        }
        // TODO update roots
        nodes[newNode.id] = newNode;
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
