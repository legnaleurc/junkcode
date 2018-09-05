import React, { Component } from 'react';
import { hot } from 'react-hot-loader';

import { Treebeard } from 'react-treebeard';


class TreeView extends Component {

  constructor (props) {
    super(props);

    this.state = {
      data: this.props.roots.map((root) => {
        return createNode(root);
      }),
    };
  }

  async onToggle (node, toggled) {
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
        onToggle={this.onToggle.bind(this)}
      />
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
