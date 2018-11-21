import React from 'react';
import { connect } from 'react-redux';

import Button from './button';
import { postSync } from '../states/file_system/actions';


class ActionBar extends React.Component {

  constructor (props) {
    super(props);

    this._sync = this._sync.bind(this);
  }

  render () {
    return (
      <div className="action-bar">
        <Button onClick={this._sync}>sync</Button>
      </div>
    );
  }

  _sync () {
    const { sync } = this.props;
    sync();
  }

}


function mapDispatchToProps (dispatch) {
  return {
    sync () {
      dispatch(postSync());
    },
  };
}


export default connect(null, mapDispatchToProps)(ActionBar);
