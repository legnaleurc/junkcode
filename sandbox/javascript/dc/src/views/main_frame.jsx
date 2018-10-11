import React from 'react';
import { connect } from 'react-redux';
import { hot } from 'react-hot-loader';

import FileExplorer from './file_explorer';
import { getRootList } from '../states/file_system/actions';


class MainFrame extends React.Component {

  constructor (props) {
    super(props);
  }

  componentDidMount() {
    const { fileSystem, dispatch } = this.props;
    dispatch(getRootList(fileSystem));
  }

  render () {
    const { fileSystem, root } = this.props;
    return (
      <div>
        <FileExplorer fileSystem={fileSystem} root={root} />
      </div>
    );
  }

}


function mapStateToProps (state) {
  return {
    root: state.fileSystem.roots.map(id => {
      return state.fileSystem.nodes[id];
    }),
  };
}


export default hot(module)(connect(mapStateToProps)(MainFrame));
