import React from 'react';
import { connect } from 'react-redux';

import { classNameFromObject } from '../lib';
import { getList, getStreamUrl } from '../states/file_system/actions';
import {
  toggleSelection,
  moveSelectedNodesTo,
} from '../states/selection/actions';

import './tree_node.css';


class TreeNode extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      expended: false,
      dragOver: false,
    };

    this._onDragStart = this._onDragStart.bind(this);
    this._onDragEnter = this._onDragEnter.bind(this);
    this._onDragOver = this._onDragOver.bind(this);
    this._onDragExit = this._onDragExit.bind(this);
    this._onDrop = this._onDrop.bind(this);
  }

  render () {
    const { node, selected } = this.props;
    const { dragOver } = this.state;
    return (
      <Dragable
        enabled={selected}
        onDragStart={this._onDragStart}
        onDragEnter={this._onDragEnter}
        onDragOver={this._onDragOver}
        onDragExit={this._onDragExit}
        onDrop={this._onDrop}
      >
        <div className={classNameFromObject({
          'tree-node': true,
          'drag-over': dragOver,
        })}>
          <div className={classNameFromObject({
            head: true,
            selected,
          })}>
            {this._renderIndicator()}
            <div
              className={classNameFromObject({
                shift: !node.children,
              })}
              onClick={event => {
                event.preventDefault();
                this._toggleSelection();
              }}
              onDoubleClick={event => {
                event.preventDefault();
                this._openFile();
              }}
            >
              {node.name}
            </div>
          </div>
          {this._renderChildren()}
        </div>
      </Dragable>
    );
  }

  _renderIndicator () {
    const { node } = this.props;
    const { expended } = this.state;
    const { children } = node;

    if (!children) {
      return null;
    }
    return (
      <Indicator
        expended={expended}
        onClick={event => {
          event.preventDefault();
          this._toggle();
        }}
      />
    );
  }

  _renderChildren () {
    const { node } = this.props;
    const { expended } = this.state;
    const { children } = node;

    if (!children || children.length <= 0) {
      return null;
    }

    return (
      <div className={classNameFromObject({
        tail: true,
        hidden: !expended,
      })}>
        <div>
          {children.map((nodeId, index) => (
            <div key={index}>
              <ConnectedTreeNode nodeId={nodeId} />
            </div>
          ))}
        </div>
      </div>
    );
  }

  _toggle () {
    const { node, getChildren } = this.props;
    if (!node.fetched) {
      getChildren(node.id);
    }

    const { expended } = this.state;
    // flip
    this.setState({
      expended: !expended,
    });
  }

  _openFile () {
    const { getFileUrl, node } = this.props;
    getFileUrl(node.id, openUrl);
  }

  _toggleSelection () {
    const { node, toggleSelection } = this.props;
    toggleSelection(node.id);
  }

  _onDragStart (event) {
    event.stopPropagation();
    const { node } = this.props;
    event.dataTransfer.dropEffect = 'copy';
    event.dataTransfer.setData('text/plain', node.id);
  }

  _onDragEnter (event) {
    event.preventDefault();
    event.stopPropagation();
    this.setState({
      dragOver: true,
    });
  }

  _onDragExit (event) {
    event.preventDefault();
    event.stopPropagation();
    this.setState({
      dragOver: false,
    });
  }

  _onDragOver (event) {
    event.preventDefault();
    event.stopPropagation();
  }

  _onDrop (event) {
    event.preventDefault();
    event.stopPropagation();
    this.setState({
      dragOver: false,
    });
    const { node, moveSelectedNodesTo } = this.props;
    if (node.children) {
      moveSelectedNodesTo(node.id);
    } else {
      moveSelectedNodesTo(node.parent_id);
    }
  }

}


function Indicator (props) {
  return <div className={classNameFromObject({
    indicator: true,
    expended: props.expended,
  })} onClick={props.onClick} />;
}


function openUrl (url) {
  function onCopy (event) {
    event.preventDefault();
    event.clipboardData.setData('text/plain', url);
    document.removeEventListener('copy', onCopy);
  }
  document.addEventListener('copy', onCopy);
  document.execCommand('copy');
}


class Dragable extends React.Component {

  constructor (props) {
    super(props);
  }

  render() {
    const {
      enabled,
      children,
      onDragStart,
      onDragEnter,
      onDragOver,
      onDragExit,
      onDrop,
    } = this.props;
    return (
      <div
        draggable={enabled}
        onDragStart={onDragStart}
        onDragEnter={onDragEnter}
        onDragOver={onDragOver}
        onDragExit={onDragExit}
        onDrop={onDrop}
      >
        {children}
      </div>
    );
  }

}


function mapStateToProps (state, ownProps) {
  const { fileSystem, selection } = state;
  const { nodeId } = ownProps;

  return {
    node: fileSystem.nodes[nodeId],
    selected: !!selection.selection[nodeId],
  };
}


function mapDispatchToProps (dispatch, ownProps) {
  return {
    getChildren (id) {
      dispatch(getList(id));
    },
    getFileUrl (id, done) {
      dispatch(getStreamUrl(id, done));
    },
    toggleSelection (id) {
      dispatch(toggleSelection(id));
    },
    moveSelectedNodesTo (id) {
      dispatch(moveSelectedNodesTo(id));
    },
  };
}


const ConnectedTreeNode = connect(mapStateToProps, mapDispatchToProps)(TreeNode);
export default ConnectedTreeNode;
