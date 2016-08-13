import React from 'react';
import ReactDOM from 'react-dom';

import {WeaponInputView} from './view.jsx';


function main () {
  var test = document.querySelector('#test');

  ReactDOM.render(<WeaponInputView/>, test);

  var tmp = location.hash.substr(1);
  if (tmp) {
    tmp = loadFromBase64(tmp);
  }
  window.tmp = tmp;
}


function loadFromBase64 (b64) {
  var json = atob(b64);
  json = escape(json);
  json = decodeURIComponent(json);
  json = JSON.parse(json);
  return json;
}

main();
