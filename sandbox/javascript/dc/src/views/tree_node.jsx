import React from 'react';
import { connect } from 'react-redux';

import { classNameFromObject } from '../lib';
import { getList } from '../states/file_system/actions';

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
    const { children, fileSystem } = this.props;
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
              <ConnectedTreeNode fileSystem={fileSystem} node={node} />
            </div>
          ))}
        </div>
      </div>
    );
  }

  _toggle () {
    const { getChildren } = this.props;
    getChildren();

    const { expended } = this.state;
    // flip
    this.setState({
      expended: !expended,
    });
  }

  _openFile () {
    const { fileSystem, node } = this.props;
    window.open(fileSystem.file(node.id), '_blank');
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


function mapStateToProps (state, ownProps) {
  const { node } = ownProps;
  return {
    children: node.children && node.children.map(id => state.fileSystem.nodes[id]),
  };
}


function mapDispatchToProps (dispatch, ownProps) {
  return {
    getChildren () {
      const { fileSystem, node } = ownProps;
      if (!node.fetched) {
        dispatch(getList(fileSystem, node.id));
      }
    }
  };
}


const ConnectedTreeNode = connect(mapStateToProps, mapDispatchToProps)(TreeNode);
export default ConnectedTreeNode;
