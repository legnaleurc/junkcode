import React from 'react';

import { classNameFromObject } from '../lib';


const Context = React.createContext();


class SwitchBar extends React.Component {

  constructor (props) {
    super(props);

    this._select = this._select.bind(this);
  }

  render () {
    return (
      <div className="switch-bar">
        <Context.Provider value={{
            selected: this.props.selected,
            select: this._select,
          }}
        >
          {this.props.children}
        </Context.Provider>
      </div>
    );
  }

  _select (name) {
    this.props.onSwitch(name);
  }

}


SwitchBar.Switch = class Switch extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    return (
      <Context.Consumer>
        {(value) => (
          <div
            className={classNameFromObject({
              switch: true,
              active: value.selected === this.props.name,
            })}
            onClick={event => {
              value.select(this.props.name);
            }}
          >
            {this.props.children}
          </div>
        )}
      </Context.Consumer>
    );
  }

}


export default SwitchBar;
