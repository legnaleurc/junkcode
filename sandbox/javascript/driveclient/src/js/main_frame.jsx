import React, { Component } from 'react';
import { hot } from 'react-hot-loader';


class MainFrame extends Component {

  constructor () {
    super();
  }

  render () {
    return (
      <h1>Hello, world!</h1>
    );
  }

}


export default hot(module)(MainFrame);
