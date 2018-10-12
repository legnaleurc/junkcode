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
    const { getRoots } = this.props;
    getRoots();
  }

  render () {
    const { root } = this.props;
    return (
      <div>
        <FileExplorer root={root} />
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


function mapDispatchToProps (dispatch) {
  return {
    getRoots () {
      dispatch(getRootList());
    }
  };
}


export default hot(module)(connect(mapStateToProps, mapDispatchToProps)(MainFrame));
