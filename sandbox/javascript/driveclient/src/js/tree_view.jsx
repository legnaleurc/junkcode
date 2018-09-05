import React from 'react';
import { hot } from 'react-hot-loader';

import { Treebeard, decorators } from 'react-treebeard';


class TreeView extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      data: this.props.roots.map((root) => {
        return createNode(root);
      }),
    };
  }

  async _onToggle (node, toggled) {
    if (this.state.cursor) {
      this.state.cursor.active = false;
    }

    node.active = true;

    if (node.children && !node.fetched) {
      const children = await this.props.fileSystem.list(node.id);

      for (const child of children) {
        node.children.push(createNode(child));
      }

      node.fetched = true;
    }

    if (node.children) {
      node.toggled = toggled;
    }

    this.setState({
      cursor: node,
    });
  }

  render () {
    return (
      <Treebeard
        data={this.state.data}
        onToggle={this._onToggle.bind(this)}
        decorators={{
          Loading: decorators.Loading,
          Toggle: decorators.Toggle,
          Header: decorators.Header,
          Container: Container,
        }}
      />
    );
  }

}


class Container extends React.Component {

  render () {
    const classes = ['draggable-container'];
    if (this.props.terminal) {
      classes.push('file');
    }
    return (
      <div
        draggable
        className={classes.join(' ')}
      >
        <decorators.Container {...this.props} />
      </div>
    );
  }

}


function createNode (data) {
  return {
    toggled: false,
    fetched: false,
    id: data.id,
    name: data.name,
    children: data.is_folder ? [] : null,
  };
}


export default hot(module)(TreeView);
