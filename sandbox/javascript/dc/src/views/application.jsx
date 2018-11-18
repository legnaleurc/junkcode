import React from 'react';
import { connect } from 'react-redux';
import { hot } from 'react-hot-loader';

import ActionBar from './action_bar';
import FileExplorer from './file_explorer';
import SwitchBar from './switch_bar';
import MutexView from './mutex_view';
import { getRootList } from '../states/file_system/actions';

import './application.scss';


class Application extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      selected: 'normal',
    };
  }

  componentDidMount() {
    const { getRoots } = this.props;

    getRoots();
  }

  render () {
    const { rootList } = this.props;
    return (
      <div className="application">
        <div className="side-bar">
          <SwitchBar onSwitch={(key) => {
            this.setState({
              selected: key,
            });
          }}>
            <SwitchBar.Switch name="normal" default>
              <span>normal</span>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="two-pane">
              <span>two-pane</span>
            </SwitchBar.Switch>
          </SwitchBar>
        </div>
        <div className="content">
          <MutexView selected={this.state.selected}>
            <MutexView.Mutex name="normal">
              <NormalView rootList={rootList} />
            </MutexView.Mutex>
            <MutexView.Mutex name="two-pane">
              <TwoPaneView rootList={rootList} />
            </MutexView.Mutex>
          </MutexView>
        </div>
      </div>
    );
  }

}


function NormalView (props) {
  const { rootList } = props;
  return (
    <div className="normal-view">
      <div className="header">
        <ActionBar />
      </div>
      <div className="content">
        <div className="pane">
          <FileExplorer root={rootList} />
        </div>
      </div>
    </div>
  );
}


function TwoPaneView (props) {
  const { rootList } = props;
  return (
    <div className="two-pane-view">
      <div className="header">
        <ActionBar />
      </div>
      <div className="content">
        <div className="pane">
          <FileExplorer root={rootList} />
        </div>
        <div className="pane">
          <FileExplorer root={rootList} />
        </div>
      </div>
    </div>
  );
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
