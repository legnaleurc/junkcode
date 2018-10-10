import React from 'react';
import ReactDOM from 'react-dom';

import MainFrame from './main_frame';
import { FileSystem, createNode } from './lib';


async function main () {
  const fileSystem = new FileSystem('http://localhost:9000');
  const rootId = await fileSystem.root();
  let root = await fileSystem.list(rootId);
  root = root.map(node => createNode(node));
  ReactDOM.render(<MainFrame fileSystem={fileSystem} root={root} />, document.querySelector('#main-window'));
}


main();
