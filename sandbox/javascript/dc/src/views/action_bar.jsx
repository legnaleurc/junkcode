import React from 'react';
import { connect } from 'react-redux';

import Button from './button';
import { postSync } from '../states/file_system/actions';
import {
  deleteSelectedNodes,
  viewSelectedNode,
} from '../states/selection/actions';

import './action_bar.scss';


class ActionBar extends React.Component {

  constructor (props) {
    super(props);

    this._sync = this._sync.bind(this);
    this._trash = this._trash.bind(this);
    this._comic = this._comic.bind(this);
  }

  render () {
    return (
      <div className="action-bar">
        <div className="group">
          <Button onClick={this._sync}>SYNC</Button>
        </div>
        <div className="group">
          <Button onClick={this._comic}>MPV</Button>
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

  _comic () {
    const { comic } = this.props;
    comic();
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
    comic () {
      dispatch(viewSelectedNode());
    },
  };
}


export default connect(null, mapDispatchToProps)(ActionBar);
