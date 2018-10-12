import React from 'react';
import { connect } from 'react-redux';

import { classNameFromObject } from '../lib';
import { getList, getFile } from '../states/file_system/actions';

import './tree_node.css';


class TreeNode extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      expended: false,
    };

    this._toggle = this._toggle.bind(this);
    this._openFile = this._openFile.bind(this);
  }

  render () {
    const { node } = this.props;
    return (
      <div className='tree-node'>
        <div className='head'>
          {this._renderIndicator()}
          <div onDoubleClick={this._openFile}>{node.name}</div>
        </div>
        {this._renderChildren()}
      </div>
    );
  }

  _renderIndicator () {
    const { children } = this.props;
    const { expended } = this.state;

    if (children) {
      return <Indicator expended={expended} onClick={this._toggle} />;
    } else {
      return <Placeholder />;
    }
  }

  _renderChildren () {
    const { children } = this.props;
    const { expended } = this.state;

    if (!children || children.length <= 0) {
      return null;
    }

    return (
      <div className={classNameFromObject({
        tail: true,
        hidden: !expended,
      })}>
        <Placeholder />
        <div>
          {children.map((node, index) => (
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

}


function Placeholder (props) {
  return <div className="placeholder" />;
}


function Indicator (props) {
  return <div className={classNameFromObject({
    indicator: true,
    expended: props.expended,
  })} onClick={props.onClick} />;
}


function openUrl (url) {
  window.open(url, '_blank');
}


function mapStateToProps (state, ownProps) {
  const { node } = ownProps;
  return {
    children: node.children && node.children.map(id => state.fileSystem.nodes[id]),
  };
}


function mapDispatchToProps (dispatch, ownProps) {
  return {
    getChildren (id) {
      dispatch(getList(id));
    },
    getFileUrl (id, done) {
      dispatch(getFile(id, done));
    },
  };
}


const ConnectedTreeNode = connect(mapStateToProps, mapDispatchToProps)(TreeNode);
export default ConnectedTreeNode;
