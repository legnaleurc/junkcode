import React from 'react';
import { connect } from 'react-redux';

import { classNameFromObject } from '../lib/index';
import { toggleSelection } from '../states/selection/actions';


class Area extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { children, selected } = this.props;
    return (
      <div className={classNameFromObject({
        'selectable-area': true,
        selected,
      })}>
        {children}
      </div>
    );
  }

}


class Trigger extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { children } = this.props;
    return (
      <div
        className="selectable-trigger"
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

  _multiSelect () {
    const { nodeId, onMultiSelect } = this.props;
    if (onMultiSelect) {
      onMultiSelect(nodeId);
    }
  }

}


function mapStateToProps (state, ownProps) {
  const { selection } = state;
  const { nodeId } = ownProps;
  return {
    selected: !!selection.table[nodeId],
  };
}


function mapDispatchToProps (dispatch) {
  return {
    toggleSelection (id) {
      dispatch(toggleSelection(id));
    },
  };
}


export default {
  Area: connect(mapStateToProps, undefined)(Area),
  Trigger: connect(undefined, mapDispatchToProps)(Trigger),
};
