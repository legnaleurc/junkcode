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

  constructor (props) {
    super(props);

    this.state = {
      highlight: false,
    };
  }

  render () {
    const classes = ['draggable-container'];
    if (this.props.terminal) {
      classes.push('file');
    }
    if (this.state.highlight) {
      classes.push('highlight');
    }
    return (
      <div
        className={classes.join(' ')}
        draggable
        onDragStart={this._onDragStart.bind(this)}
        onDragEnter={this._onDragEnter.bind(this)}
        onDragOver={this._onDragOver.bind(this)}
        onDragLeave={this._onDragLeave.bind(this)}
        onDrop={this._onDrop.bind(this)}
      >
        <decorators.Container {...this.props} />
      </div>
    );
  }

  _onDragStart (event) {
    // event.preventDefault();
    const node = this.props.node;
    event.dataTransfer.setData('text/plain', node.id);
  }

  _onDragEnter (event) {
    event.preventDefault();
    this.setState({
      highlight: true,
    });
    console.info('enter', this.props.node.name, event.target);
  }

  _onDragOver (event) {
    event.preventDefault();
  }

  _onDragLeave (event) {
    event.preventDefault();
    this.setState({
      highlight: false,
    });
    console.info('leave', this.props.node.name, event.target);
  }

  _onDrop (event) {
    event.preventDefault();
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
