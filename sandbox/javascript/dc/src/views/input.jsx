import React from 'react';

import './input.scss';


export default function Input (props) {
  return (
    <input
      className="input"
      type={props.type}
      onKeyPress={props.onKeyPress}
    />
  );
};
