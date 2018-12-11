import React from 'react';
import { connect } from 'react-redux';


import ContentActionBar from './content_action_bar';
import FileExplorer from './file_explorer';
import FileSystemActionBar from './file_system_action_bar';
import SortActionBar from './sort_action_bar';

import './double_tree_view.scss';


function DoubleTreeView (props) {
  return (
    <div className="double-tree-view">
      <div className="head">
        <div className="group">
          <FileSystemActionBar />
        </div>
        <div className="group">
          <SortActionBar />
        </div>
        <div className="group">
          <ContentActionBar />
        </div>
      </div>
      <div className="tail">
        <div className="group">
          <FileExplorer rootId={props.rootId} />
        </div>
        <div className="group">
          <FileExplorer rootId={props.rootId} />
        </div>
      </div>
    </div>
  );
}


function mapStateToProps (state) {
  return {
    rootId: state.fileSystem.rootId,
  };
}


export default connect(mapStateToProps)(DoubleTreeView);
