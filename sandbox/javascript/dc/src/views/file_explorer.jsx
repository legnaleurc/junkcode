import React from 'react';
import { connect } from 'react-redux';

import TreeNode from './tree_node';


class FileExplorer extends React.Component {

  constructor (props) {
    super(props);
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


export default connect(mapStateToProps)(FileExplorer);
