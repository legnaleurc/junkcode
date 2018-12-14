import React from 'react';
import { connect } from 'react-redux';

import Button from './button';
import { setSortFunction } from '../states/file_system/actions';
import {
  SORT_BY_NAME_ASC,
  SORT_BY_MTIME_ASC,
  SORT_BY_MTIME_DES,
} from '../states/file_system/sort';


class SortActionBar extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { setSortFunction } = this.props;
    return (
      <div className="sort-action-bar">
        <div className="group">
          <Button
            onClick={event => {
              setSortFunction(SORT_BY_NAME_ASC);
            }}
          >
            SORT_BY_NAME_ASC
          </Button>
          <Button
            onClick={event => {
              setSortFunction(SORT_BY_MTIME_ASC);
            }}
          >
            SORT_BY_MTIME_ASC
          </Button>
          <Button
            onClick={event => {
              setSortFunction(SORT_BY_MTIME_DES);
            }}
          >
            SORT_BY_MTIME_DES
          </Button>
        </div>
      </div>
    );
  }

}


function mapDispatchToProps (dispatch) {
  return {
    setSortFunction (key) {
      dispatch(setSortFunction(key));
    },
  };
}


export default connect(null, mapDispatchToProps)(SortActionBar);
