import React from 'react';

import TreeNode from './tree_node';


class FileExplorer extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { root } = this.props;
    if (!Array.isArray(root)) {
      return <TreeNode node={root} />;
    }
    return root.map((node, index) => (
      <div key={index}>
        <TreeNode node={node} />
      </div>
    ));
  }

}


export default FileExplorer;
