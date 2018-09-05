import React from 'react';
import ReactDOM from 'react-dom';

import { FileSystem } from './lib.js';
import MainFrame from './main_frame.jsx';


async function main () {
  const rfs = new FileSystem('http://localhost:8000');
  const children = await rfs.list('/');
  const el = document.querySelector('#main-window');
  ReactDOM.render(<MainFrame
    fileSystem={rfs}
    roots={children}
  />, el);
}


main().catch((e) => {
  console.error(e);
});
