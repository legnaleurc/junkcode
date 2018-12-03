import React from 'react';

import TreeNode from './tree_node';


class FileExplorer extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { rootId } = this.props;
    return rootId ? <TreeNode nodeId={rootId} /> : null;
    const { root } = this.props;
    if (!Array.isArray(root)) {
      return <TreeNode nodeId={root} />;
    }
    return (
      <div className="file-explorer">
        {root.map((nodeId, index) => (
          <React.Fragment key={index}>
            <TreeNode nodeId={nodeId} />
          </React.Fragment>
        ))}
      </div>
    );
  }

}


export default FileExplorer;
