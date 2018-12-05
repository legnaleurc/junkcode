import React from 'react';

import { classNameFromObject } from '../lib';

import './dragdrop.scss';


class Dragable extends React.Component {

  constructor (props) {
    super(props);

    this._onDragStart = this._onDragStart.bind(this);
  }

  render() {
    const {
      enabled,
      children,
    } = this.props;
    return (
      <div
        className="dragable"
        draggable={enabled}
        onDragStart={this._onDragStart}
      >
        {children}
      </div>
    );
  }

  _onDragStart (event) {
    event.stopPropagation();
    const { onDragStart } = this.props;
    onDragStart(event);
  }

}


class Dropable extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      dragOver: false,
    };

    this._dragCounter = 0;

    this._onDragEnter = this._onDragEnter.bind(this);
    this._onDragOver = this._onDragOver.bind(this);
    this._onDragLeave = this._onDragLeave.bind(this);
    this._onDrop = this._onDrop.bind(this);
  }

  render() {
    const { children } = this.props;
    const { dragOver } = this.state;
    return (
      <div
        className={classNameFromObject({
          'dropable': true,
          'drag-over': dragOver,
        })}
        onDragEnter={this._onDragEnter}
        onDragOver={this._onDragOver}
        onDragLeave={this._onDragLeave}
        onDrop={this._onDrop}
      >
        {children}
      </div>
    );
  }

  _onDragEnter (event) {
    event.preventDefault();
    event.stopPropagation();
    if (this._dragCounter === 0) {
      this.setState({
        dragOver: true,
      });
    }
    this._dragCounter += 1;
  }

  _onDragLeave (event) {
    event.preventDefault();
    event.stopPropagation();
    this._dragCounter -= 1;
    if (this._dragCounter === 0) {
      this.setState({
        dragOver: false,
      });
    }
  }

  _onDragOver (event) {
    event.preventDefault();
    event.stopPropagation();
  }

  _onDrop (event) {
    event.preventDefault();
    event.stopPropagation();
    this._dragCounter = 0;
    this.setState({
      dragOver: false,
    });
    const { onDrop } = this.props;
    onDrop(event);
  }

}


export default { Dragable, Dropable };
