import React from 'react';
import { connect } from 'react-redux';

import { classNameFromObject } from '../lib';
import { getList, getStreamUrl } from '../states/file_system/actions';
import { toggleSelection } from '../states/selection/actions';

import './tree_node.css';


class TreeNode extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      expended: false,
    };

    this._onDragStart = this._onDragStart.bind(this);
    this._onDragOver = this._onDragOver.bind(this);
    this._onDrop = this._onDrop.bind(this);
  }

  render () {
    const { node, selected } = this.props;
    return (
      <Dragable
        enabled={selected}
        onDragStart={this._onDragStart}
        onDragOver={this._onDragOver}
        onDrop={this._onDrop}
      >
        <div className='tree-node'>
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
    console.info('drag start', event.dataTransfer);
    event.dataTransfer.dropEffect = 'copy';
    event.dataTransfer.setData('text/plain', '?');
  }

  _onDragOver (event) {
    const { node } = this.props;
    console.info('drag over', node.name);
    event.preventDefault();
  }

  _onDrop (event) {
    const { node } = this.props;
    console.info('drop', node.name);
    event.preventDefault();
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
    const { enabled, children, onDragStart, onDragOver, onDrop } = this.props;
    return (
      <div
        draggable={enabled}
        onDragStart={onDragStart}
        onDragOver={onDragOver}
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
  };
}


const ConnectedTreeNode = connect(mapStateToProps, mapDispatchToProps)(TreeNode);
export default ConnectedTreeNode;
