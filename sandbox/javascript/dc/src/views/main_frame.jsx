import React from 'react';
import { connect } from 'react-redux';
import { hot } from 'react-hot-loader';

import FileExplorer from './file_explorer';
import SearchList from './search_list';
import { getRootList, postSync, upsertNode } from '../states/file_system/actions';


class MainFrame extends React.Component {

  constructor (props) {
    super(props);

    this._sync = this._sync.bind(this);
    this._onMessage = this._onMessage.bind(this);
  }

  componentDidMount() {
    const { getRoots, connectSocket } = this.props;

    getRoots();

    this._connection = connectSocket({
      onMessage: this._onMessage,
      onClose: this._onClose,
      onError: this._onError,
    });
  }

  render () {
    const { root } = this.props;
    return (
      <div>
        <div>
          <button onClick={this._sync}>sync</button>
        </div>
        <SearchList />
        <FileExplorer root={root} />
      </div>
    );
  }

  _sync () {
    const { startSync } = this.props;
    startSync();
  }

  _onMessage (event) {
    let message = event.data;
    message = JSON.parse(message);
    const { applyChange } = this.props;
    applyChange(message);
  }

  _onClose (e) {
    console.info(e);
  }

  _onError (e) {
    console.info(e);
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
    },
    startSync () {
      dispatch(postSync());
    },
    applyChange (change) {
      dispatch(upsertNode(change));
    },
  };
}


export default hot(module)(connect(mapStateToProps, mapDispatchToProps)(MainFrame));
