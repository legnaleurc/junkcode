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
  }

  shouldComponentUpdate(nextProps, nextState, nextContext) {
    if (this.props.node !== nextProps.node) {
      console.info('node', this.props.node.name);
      return true;
    }
    if (this.props.node.children !== nextProps.node.children) {
      console.info('childNodes', this.props.node.name);
      return true;
    }
    if (this.props.selected !== nextProps.selected) {
      console.info('selected', this.props.node.name);
      return true;
    }
    if (this.state.expended !== nextState.expended) {
      console.info('expended', this.props.node.name);
      return true;
    }
    return false;
  }

  render () {
    const { node, childNodes, selected } = this.props;
    return (
      <div className='tree-node'>
        <div className={classNameFromObject({
          head: true,
          selected,
        })}>
          {this._renderIndicator()}
          <div
            className={classNameFromObject({
              shift: !childNodes,
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
    );
  }

  _renderIndicator () {
    const { childNodes } = this.props;
    const { expended } = this.state;

    if (!childNodes) {
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
    const { childNodes } = this.props;
    const { expended } = this.state;

    if (!childNodes || childNodes.length <= 0) {
      return null;
    }

    return (
      <div className={classNameFromObject({
        tail: true,
        hidden: !expended,
      })}>
        <div>
          {childNodes.map((node, index) => (
            <div key={index}>
              <ConnectedTreeNode node={node} />
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


function mapStateToProps (state, ownProps) {
  const { fileSystem, selection} = state;
  const { node } = ownProps;

  return {
    childNodes: node.children && node.children.map(id => fileSystem.nodes[id]),
    selected: !!selection.selection[node.id],
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
