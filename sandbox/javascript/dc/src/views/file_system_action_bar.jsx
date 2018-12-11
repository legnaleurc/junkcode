import React from 'react';
import { connect } from 'react-redux';

import Button from './button';
import { postSync } from '../states/file_system/actions';
import { deleteSelectedNodes } from '../states/selection/actions';

import './file_system_action_bar.scss';


class FileSystemActionBar extends React.Component {

  constructor (props) {
    super(props);

    this._sync = this._sync.bind(this);
    this._trash = this._trash.bind(this);
  }

  render () {
    return (
      <div className="file-system-action-bar">
        <div className="group">
          <Button onClick={this._sync}>SYNC</Button>
        </div>
        <div className="expand">
        </div>
        <div className="group">
          <Button onClick={this._trash}>TRASH</Button>
        </div>
      </div>
    );
  }

  _sync () {
    const { sync } = this.props;
    sync();
  }

  _trash () {
    const { trash } = this.props;
    trash();
  }

}


function mapDispatchToProps (dispatch) {
  return {
    sync () {
      dispatch(postSync());
    },
    trash () {
      dispatch(deleteSelectedNodes());
    },
  };
}


export default connect(null, mapDispatchToProps)(FileSystemActionBar);
