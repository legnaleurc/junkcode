import {
  FS_ROOT_GET_SUCCEED,
  FS_LIST_GET_SUCCEED,
  FS_SYNC_SUCCEED,
  FS_SET_SORT,
} from './actions';
import {
  SORT_BY_MTIME_DES,
  getCompareFunction,
} from './sort';


const initialState = {
  nodes: {},
  rootId: null,
  sortKey: SORT_BY_MTIME_DES,
};


export default function reduceFileSystem (state = initialState, { type, payload }) {
  switch (type) {
    case FS_ROOT_GET_SUCCEED: {
      let { node } = payload;
      const { children } = payload;

      const cmp = getCompareFunction(state.sortKey);
      children.sort(cmp);

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

      const cmp = getCompareFunction(state.sortKey);
      children.sort(cmp);

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
    case FS_SYNC_SUCCEED: {
      const { nodes } = state;
      const { changeList } = payload;
      for (const change of changeList) {
        applyChange(nodes, state.sortKey, change);
      }
      return Object.assign({}, state, {
        nodes: Object.assign({}, nodes),
      });
    }
    case FS_SET_SORT: {
      const { nodes, rootId } = state;
      const { key } = payload;

      if (key === state.sortKey) {
        return state;
      }

      const cmp = getCompareFunction(key);
      deepSort(nodes, rootId, cmp);

      return Object.assign({}, state, {
        nodes: Object.assign({}, nodes),
        sortKey: key,
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
    modified: node.modified,
    fetched: false,
  };
}


function applyChange (nodes, sortKey, change) {
  if (change.removed) {
    removeNode(nodes, change.id);
    return;
  }
  if (change.node.trashed) {
    removeNode(nodes, change.node.id);
    return;
  }
  upsertNode(nodes, sortKey, change.node);
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


function upsertNode (nodes, sortKey, node) {
  const newNode = createNode(node);
  node = nodes[node.id];

  const cmp = getCompareFunction(sortKey);

  // this is a new node
  if (!node) {
    // if have parent and already fetched chilidren, need to update the list
    let parent = nodes[newNode.parentId];
    if (parent && parent.fetched) {
      parent = Object.assign({}, parent, {
        children: [newNode.id, ...parent.children].sort(cmp),
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
        children: [newNode.id, ...parent.children].sort(cmp),
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


function deepSort (nodes, id, cmp) {
  const node = nodes[id];
  if (!node) {
    return;
  }
  if (!node.fetched) {
    return;
  }
  const children = node.children.map(id => nodes[id]);
  children.sort(cmp);
  nodes[id] = {
    ...node,
    children: children.map(node => node.id),
  };
  for (const node of children) {
    deepSort(nodes, node.id, cmp);
  }
}
