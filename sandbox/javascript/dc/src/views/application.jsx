import React from 'react';
import { connect } from 'react-redux';
import { hot } from 'react-hot-loader';

import FileExplorer from './file_explorer';
import { getRootList, postSync } from '../states/file_system/actions';

import './application.css';


class Application extends React.Component {

  constructor (props) {
    super(props);

    this._sync = this._sync.bind(this);
  }

  componentDidMount() {
    const { getRoots } = this.props;

    getRoots();
  }

  render () {
    const { rootList } = this.props;
    return (
      <div className="flex-container flex-vertical full-width full-height">
        <div>
          <button onClick={this._sync}>sync</button>
        </div>
        <div className="flex-container full-width full-height">
          <FileExplorer className="tmp" root={rootList} />
          <FileExplorer className="tmp" root={rootList} />
        </div>
      </div>
    );
  }

  _sync () {
    const { sync } = this.props;
    sync();
  }

}


function mapStateToProps (state) {
  return {
    rootList: state.fileSystem.roots,
  };
}


function mapDispatchToProps (dispatch) {
  return {
    getRoots () {
      dispatch(getRootList());
    },
    sync () {
      dispatch(postSync());
    },
  };
}


export default hot(module)(connect(mapStateToProps, mapDispatchToProps)(Application));
