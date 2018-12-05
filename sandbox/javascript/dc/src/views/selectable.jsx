import React from 'react';
import { connect } from 'react-redux';

import {
  toggleSelection,
  continuouslySelect,
} from '../states/selection/actions';


class Selectable extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { children } = this.props;
    return (
      <div
        className="selectable"
        onClick={event => {
          event.preventDefault();
          if (event.shiftKey) {
            this._continuouslySelect();
          } else {
            this._toggleSelection();
          }
        }}
      >
        {children}
      </div>
    );
  }

  _toggleSelection () {
    const { nodeId, toggleSelection } = this.props;
    toggleSelection(nodeId);
  }

  _continuouslySelect() {
    const { nodeId, continuouslySelect } = this.props;
    continuouslySelect(nodeId);
  }

}


function mapDispatchToProps (dispatch) {
  return {
    toggleSelection (id) {
      dispatch(toggleSelection(id));
    },
    continuouslySelect (id) {
      dispatch(continuouslySelect(id));
    },
  };
}


export default connect(undefined, mapDispatchToProps)(Selectable);
