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
      const { nodes } = state;
      const { id, children } = payload;
      for (const node of children) {
        nodes[node.id] = createNode(node);
      }
      const parent = nodes[id];
      nodes[id] = Object.assign({}, parent, {
        fetched: true,
        children: children.map(node => node.id),
      });
      return Object.assign({}, state, {
        nodes: Object.assign({}, nodes),
      });
    }
    case SYNC_POST_SUCCEED: {
      const { nodes } = state;
      const { changeList } = payload;
      console.info('apply start');
      for (const change of changeList) {
        applyChange(nodes, change);
      }
      console.info('apply ok');
      return Object.assign({}, state, {
        nodes: Object.assign({}, nodes),
      });
    }
    default:
      return state;
  }
}


function applyChange (nodes, change) {
  if (change.removed) {
    removeNode(nodes, change.id);
  } else {
    upsertNode(nodes, change.node);
  }
}


function removeNode (nodes, nodeId) {
  const node = nodes[nodeId];
  let parent = nodes[node.parent_id];
  if (parent) {
    parent = Object.assign({}, parent, {
      children: parent.children.filter(id => id !== nodeId),
    });
    nodes[node.parent_id] = parent;
  }
  delete nodes[nodeId];
}


// TODO update roots
function upsertNode (nodes, node) {
  const newNode = createNode(node);
  node = nodes[node.id];

  // this is a new node
  if (!node) {
    // if have parent and already fetched chilidren, need to update the list
    let parent = nodes[newNode.parent_id];
    if (parent && parent.fetched) {
      parent = Object.assign({}, parent, {
        children: [newNode.id, ...parent.children],
      });
    }
    nodes[newNode.id] = newNode;
    return;
  }

  // this is an existing node
  if (node.parent_id !== newNode.parent_id) {
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
  nodes[newNode.id] = newNode;
}
