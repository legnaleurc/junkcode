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
    return (
      <div className="file-explorer">
        {root.map((nodeId, index) => (
          <div key={index}>
            <TreeNode nodeId={nodeId} />
          </div>
        ))}
      </div>
    );
  }

}


export default FileExplorer;
