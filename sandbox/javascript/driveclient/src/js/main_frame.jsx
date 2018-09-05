import React, { Component } from 'react';
import { hot } from 'react-hot-loader';

import { Treebeard } from 'react-treebeard';


class MainFrame extends Component {

  constructor (props) {
    super(props);

    const data = props.data;

    this.state = {
      data: {
        toggled: false,
        fetched: false,
        id: data.id,
        name: data.name || '/',
        children: data.is_folder ? [] : null,
      },
    };
  }

  async onToggle (node, toggled) {
    if (this.state.cursor) {
      this.state.cursor.active = true;
    }

    node.active = true;

    if (node.children && !node.fetched) {
      const url = new URL('http://localhost:8000/api/v1/list');
      url.searchParams.append('id_or_path', node.id);
      const rqst = new Request(url, {
        method: 'GET',
        mode: 'cors',
      });
      let rv = await fetch(rqst);
      rv = await rv.json();

      rv.sort((l, r) => {
        if (l.name < r.name) {
          return -1;
        }
        if (l.name > r.name) {
          return 1;
        }
        return 0;
      });

      for (const child of rv) {
        node.children.push({
          toggled: false,
          fetched: false,
          id: child.id,
          name: child.name,
          children: child.is_folder ? [] : null,
        });
      }
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


export default hot(module)(MainFrame);
