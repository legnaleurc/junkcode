import React from 'react';

import TreeNode from './tree_node';


class FileExplorer extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { root } = this.props;
    if (!Array.isArray(root)) {
      return <TreeNode nodeId={root} />;
    }
    return root.map((nodeId, index) => (
      <div key={index}>
        <TreeNode nodeId={nodeId} />
      </div>
    ));
  }

}


export default FileExplorer;
