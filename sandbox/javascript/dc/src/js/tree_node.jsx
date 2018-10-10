import React from 'react';

import { createNode, classNameFromObject } from './lib';

import '../css/tree_node.css';


class TreeNode extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      id: props.node.id,
      name: props.node.name,
      children: props.node.children,
      fetched: false,
      expended: false,
    };

    this._toggle = this._toggle.bind(this);
    this._renderIndicator = this._renderIndicator.bind(this);
  }

  render () {
    const { name } = this.state;
    return (
      <div className='tree-node'>
        <div className='head'>
          {this._renderIndicator()}
          <div>{name}</div>
        </div>
        {this._renderChildren()}
      </div>
    );
  }

  _renderIndicator () {
    const { children, expended } = this.state;
    if (children) {
      return <Indicator expended={expended} onClick={this._toggle} />;
    } else {
      return <Placeholder />;
    }
  }

  _renderChildren () {
    const { children, expended } = this.state;
    if (!children || children.length <= 0) {
      return null;
    }
    const { fileSystem } = this.props;
    return (
      <div className={classNameFromObject({
        tail: true,
        hidden: !expended,
      })}>
        <Placeholder />
        <div>
          {children.map((node, index) => (
            <div key={index}>
              <TreeNode fileSystem={fileSystem} node={node} />
            </div>
          ))}
        </div>
      </div>
    );
  }

  async _toggle () {
    const { fetched, expended } = this.state;
    if (!fetched) {
      await this._fetch();
    }

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


export default TreeNode;
