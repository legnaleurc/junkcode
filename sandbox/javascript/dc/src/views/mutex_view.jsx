import React from 'react';

import { classNameFromObject } from '../lib';

import './mutex_view.scss';


const Context = React.createContext();


class MutexView extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    return (
      <div className="mutex-view">
        <Context.Provider value={{
            selected: this.props.selected,
          }}
        >
          {this.props.children}
        </Context.Provider>
      </div>
    );
  }

}


MutexView.Mutex = class Mutex extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    return (
      <Context.Consumer>
        {(value) => (
          <div
            className={classNameFromObject({
              mutex: true,
              active: value.selected === this.props.name,
            })}
          >
            {this.props.children}
          </div>
        )}
      </Context.Consumer>
    );
  }

}


export default MutexView;
