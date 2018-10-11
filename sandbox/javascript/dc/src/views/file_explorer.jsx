import React from 'react';

import TreeNode from './tree_node';


class FileExplorer extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { fileSystem, root } = this.props;
    if (!Array.isArray(root)) {
      return <TreeNode fileSystem={fileSystem} node={root} />;
    }
    return root.map((node, index) => (
      <div key={index}>
        <TreeNode fileSystem={fileSystem} node={node} />
      </div>
    ));
  }

}


export default FileExplorer;
