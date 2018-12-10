import {
  FS_ROOT_GET_SUCCEED,
  FS_LIST_GET_SUCCEED,
  SYNC_POST_SUCCEED,
} from './actions';


const initialState = {
  nodes: {},
  rootId: null,
};


export default function reduceFileSystem (state = initialState, { type, payload }) {
  switch (type) {
    case FS_ROOT_GET_SUCCEED: {
      let { node } = payload;
      const { children } = payload;

      node = createNode(node);
      node.fetched = true;
      node.children = children.map(node => node.id);
      const nodes = {
        [node.id]: node,
      };

      for (const node of children) {
        nodes[node.id] = createNode(node);
      }

      // root changes means all data need reinitialize
      return Object.assign({}, state, {
        nodes,
        rootId: node.id,
      });
    }
    case FS_LIST_GET_SUCCEED: {
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
      for (const change of changeList) {
        applyChange(nodes, change);
      }
      return Object.assign({}, state, {
        nodes: Object.assign({}, nodes),
      });
    }
    default:
      return state;
  }
}


function createNode (node) {
  return {
    id: node.id,
    name: node.name,
    parentId: getParentId(node),
    children: node.is_folder ? [] : null,
    fetched: false,
  };
}


function applyChange (nodes, change) {
  if (change.removed) {
    removeNode(nodes, change.id);
    return;
  }
  if (change.node.trashed) {
    removeNode(nodes, change.node.id);
    return;
  }
  upsertNode(nodes, change.node);
}


function removeNode (nodes, nodeId) {
  const node = nodes[nodeId];
  if (!node) {
    return;
  }
  const parentId = node.parentId;
  let parent = nodes[parentId];
  if (parent && parent.fetched) {
    parent = Object.assign({}, parent, {
      children: parent.children.filter(id => id !== nodeId),
    });
    nodes[parentId] = parent;
  }
  delete nodes[nodeId];
}


function upsertNode (nodes, node) {
  const newNode = createNode(node);
  node = nodes[node.id];

  // this is a new node
  if (!node) {
    // if have parent and already fetched chilidren, need to update the list
    let parent = nodes[newNode.parentId];
    if (parent && parent.fetched) {
      parent = Object.assign({}, parent, {
        children: [newNode.id, ...parent.children],
      });
      nodes[newNode.parentId] = parent;
    }
    nodes[newNode.id] = newNode;
    return;
  }

  // this is an existing node
  if (node.parentId !== newNode.parentId) {
    // remove child from old parent
    let parent = nodes[node.parentId];
    if (parent && parent.fetched) {
      parent = Object.assign({}, parent, {
        children: parent.children.filter(id => id !== node.id),
      });
      nodes[node.parentId] = parent;
    }
    // insert to new parent
    parent = nodes[newNode.parentId];
    if (parent && parent.fetched) {
      parent = Object.assign({}, parent, {
        children: [newNode.id, ...parent.children],
      });
      nodes[newNode.parentId] = parent;
    }
  }
  nodes[newNode.id] = newNode;
}


function getParentId (rawNode) {
  let p = rawNode.parent_id;
  if (p) {
    return p;
  }
  p = rawNode.parent_list;
  if (!p || p.length <= 0) {
    return null;
  }
  return p[0];
}
