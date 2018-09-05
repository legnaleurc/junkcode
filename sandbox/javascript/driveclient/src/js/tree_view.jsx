import React, { Component } from 'react';
import { hot } from 'react-hot-loader';

import { Treebeard } from 'react-treebeard';


class TreeView extends Component {

  constructor (props) {
    super(props);

    this.state = {
      data: this.props.roots.map((root) => {
        return {
          toggled: false,
          fetched: false,
          id: root.id,
          name: root.name,
          children: root.is_folder ? [] : null,
        };
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
      children.sort((l, r) => {
        if (l.name < r.name) {
          return -1;
        }
        if (l.name > r.name) {
          return 1;
        }
        return 0;
      });

      for (const child of children) {
        node.children.push({
          toggled: false,
          fetched: false,
          id: child.id,
          name: child.name,
          children: child.is_folder ? [] : null,
        });
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


export default hot(module)(TreeView);
