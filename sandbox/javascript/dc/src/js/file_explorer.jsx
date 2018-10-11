import React from 'react';

import TreeNode from './tree_node';


class FileExplorer extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { fileSystem, root } = this.props;
    if (!Array.isArray(root)) {
      return <TreeNode fileSystem={fileSystem} nodeId={root} />;
    }
    return root.map((node, index) => (
      <div key={index}>
        <TreeNode fileSystem={fileSystem} nodeId={node} />
      </div>
    ));
  }

}


export default FileExplorer;
