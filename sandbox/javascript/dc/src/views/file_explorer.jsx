import React from 'react';
import { connect } from 'react-redux';

import { getList } from '../states/file_system/actions';
import TreeNode from './tree_node';


class FileExplorer extends React.Component {

  constructor (props) {
    super(props);
  }

  componentDidUpdate () {
    const { root, getChildren } = this.props;
    if (root && !root.fetched) {
      getChildren(root.id);
    }
  }

  render () {
    const { root } = this.props;
    if (!root) {
      return null;
    }
    if (!root.children) {
      return null;
    }
    return (
      <div className="file-explorer">
        {root.children.map((nodeId, index) => (
          <React.Fragment key={index}>
            <TreeNode nodeId={nodeId} />
          </React.Fragment>
        ))}
      </div>
    );
  }

}


function mapStateToProps (state, ownProps) {
  const { nodes } = state.fileSystem;
  const { rootId } = ownProps;
  return {
    root: rootId ? nodes[rootId] : null,
  };
}


function mapDispatchToProps (dispatch) {
  return {
    getChildren (id) {
      dispatch(getList(id));
    },
  };
}


export default connect(mapStateToProps, mapDispatchToProps)(FileExplorer);
