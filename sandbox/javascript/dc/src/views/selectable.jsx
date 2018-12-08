import React from 'react';
import { connect } from 'react-redux';

import { toggleSelection } from '../states/selection/actions';


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
            this._multiSelect();
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

  _multiSelect() {
    const { nodeId, onMultiSelect } = this.props;
    if (onMultiSelect) {
      onMultiSelect(nodeId);
    }
  }

}


function mapDispatchToProps (dispatch) {
  return {
    toggleSelection (id) {
      dispatch(toggleSelection(id));
    },
  };
}


export default connect(undefined, mapDispatchToProps)(Selectable);
