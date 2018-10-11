import React from 'react';
import { connect } from 'react-redux';

import { createNode, classNameFromObject } from './lib';
import { getList } from './actions';

import '../css/tree_node.css';


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
    const { name } = this.props;
    console.info('name', name);
    return (
      <div className='tree-node'>
        <div className='head'>
          {this._renderIndicator()}
          <div onDoubleClick={this._openFile}>{name}</div>
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
          {children.map((nodeId, index) => (
            <div key={index}>
              <TreeNode fileSystem={fileSystem} nodeId={nodeId} />
            </div>
          ))}
        </div>
      </div>
    );
  }

  _toggle () {
    const { fileSystem, nodeId, fetched, dispatch } = this.props;
    if (!fetched) {
      dispatch(getList(fileSystem, nodeId));
    }

    const { expended } = this.state;
    // flip
    this.setState({
      expended: !expended,
    });
  }

  async _fetch () {
    const { id } = this.state;
    const { fileSystem } = this.props;
    const nodeList = await fileSystem.list(id);
    const children = nodeList.map(node => createNode(node));
    this.setState({
      children,
    });
  }

  _openFile () {
    const { fileSystem } = this.props;
    const { id } = this.state;
    window.open(fileSystem.file(id), '_blank');
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
  const id = ownProps.nodeId;
  const node = state.nodes[id];
  return {
    name: node.name,
    children: node.children,
    fetched: node.fetched,
  };
}


export default connect(mapStateToProps)(TreeNode);
