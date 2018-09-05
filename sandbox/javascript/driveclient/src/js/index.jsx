import React from 'react';
import ReactDOM from 'react-dom';

import MainFrame from './main_frame.jsx';


async function main () {
  const url = new URL('http://localhost:8000/api/v1/info');
  url.searchParams.append('id_or_path', '/');
  const rqst = new Request(url, {
    method: 'GET',
    mode: 'cors',
  });
  let rv = await fetch(rqst);
  rv = await rv.json();
  const el = document.querySelector('#main-window');
  ReactDOM.render(<MainFrame
    data={rv}
  />, el);
}


main().catch((e) => {
  console.error(e);
});

