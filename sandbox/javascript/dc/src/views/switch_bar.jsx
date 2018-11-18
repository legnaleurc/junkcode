import React from 'react';

import { classNameFromObject } from '../lib';


const Context = React.createContext();


class SwitchBar extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      selected: null,
    };

    this._select = this._select.bind(this);
  }

  render () {
    return (
      <div className="switch-bar">
        <Context.Provider value={{
            selected: this.state.selected,
            select: this._select,
          }}
        >
          {this.props.children}
        </Context.Provider>
      </div>
    );
  }

  _select (key) {
    this.setState({
      selected: key,
    });
  }

}


class Switch extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    return (
      <Context.Consumer>
        {(value) => {
          <div
            className={classNameFromObject({
              switch: true,
              active: value.selected === this.props.key,
            })}
            onClick={event => {
              value.select(this.props.key);
            }}
          >
            {this.props.children}
          </div>
        }}
      </Context.Consumer>
    );
  }

}
