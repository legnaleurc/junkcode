import React from 'react';
import { connect } from 'react-redux';

import Button from './button';
import { postSync } from '../states/file_system/actions';
import {
  deleteSelectedNodes,
  viewSelectedNode,
} from '../states/selection/actions';


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
        <div>
          <Button onClick={this._sync}>sync</Button>
        </div>
        <div>
          <Button onClick={this._comic}>comic</Button>
        </div>
        <div>
          <Button onClick={this._trash}>trash</Button>
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
