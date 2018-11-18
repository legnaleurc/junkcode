import React from 'react';
import { connect } from 'react-redux';
import { hot } from 'react-hot-loader';

import ActionBar from './action_bar';
import FileExplorer from './file_explorer';
import { getRootList } from '../states/file_system/actions';

import './application.scss';


class Application extends React.Component {

  constructor (props) {
    super(props);
  }

  componentDidMount() {
    const { getRoots } = this.props;

    getRoots();
  }

  render () {
    const { rootList } = this.props;
    return (
      <div className="application">
        <div className="header">
          <ActionBar />
        </div>
        <div className="content">
          <div className="half-panel">
            <FileExplorer root={rootList} />
          </div>
          <div className="half-panel">
            <FileExplorer root={rootList} />
          </div>
        </div>
      </div>
    );
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
  };
}


export default hot(module)(connect(mapStateToProps, mapDispatchToProps)(Application));
