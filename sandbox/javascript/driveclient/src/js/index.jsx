import React from 'react';
import ReactDOM from 'react-dom';

import { FileSystem } from './lib.js';
import MainFrame from './main_frame.jsx';


async function main () {
  const rfs = new FileSystem('http://localhost:8000');
  const rv = await rfs.info('/');
  const el = document.querySelector('#main-window');
  ReactDOM.render(<MainFrame
    fileSystem={rfs}
    data={rv}
  />, el);
}


main().catch((e) => {
  console.error(e);
});

