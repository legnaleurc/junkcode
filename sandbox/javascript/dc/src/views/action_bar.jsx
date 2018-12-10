import React from 'react';
import { connect } from 'react-redux';

import Button from './button';
import { postSync } from '../states/file_system/actions';
import { deleteSelectedNodes } from '../states/selection/actions';
import { loadMultiPageViewer } from '../states/multipage/actions';

import './action_bar.scss';


class ActionBar extends React.Component {

  constructor (props) {
    super(props);

    this._sync = this._sync.bind(this);
    this._trash = this._trash.bind(this);
    this._mpv = this._mpv.bind(this);
  }

  render () {
    return (
      <div className="action-bar">
        <div className="group">
          <Button onClick={this._sync}>SYNC</Button>
        </div>
        <div className="group">
          <Button onClick={this._mpv}>MPV</Button>
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

  _mpv () {
    const { mpv } = this.props;
    mpv();
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
    mpv () {
      dispatch(loadMultiPageViewer());
    },
  };
}


export default connect(null, mapDispatchToProps)(ActionBar);
