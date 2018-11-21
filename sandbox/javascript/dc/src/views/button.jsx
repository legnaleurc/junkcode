import React from 'react';

import './button.scss';

export default function Button (props) {
  return (
    <button
      className="button"
      onClick={props.onClick}
    >
      {props.children}
    </button>
  );
}
