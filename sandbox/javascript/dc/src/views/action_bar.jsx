import React from 'react';
import { connect } from 'react-redux';

import Button from './button';
import { postSync } from '../states/file_system/actions';
import { deleteSelectedNodes } from '../states/selection/actions';


class ActionBar extends React.Component {

  constructor (props) {
    super(props);

    this._sync = this._sync.bind(this);
    this._trash = this._trash.bind(this);
  }

  render () {
    return (
      <div className="action-bar">
        <Button onClick={this._sync}>sync</Button>
        <Button onClick={this._trash}>trash</Button>
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


export default connect(null, mapDispatchToProps)(ActionBar);
