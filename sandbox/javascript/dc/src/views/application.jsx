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
              <div className="switch normal"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="two-pane">
              <div className="switch two-pane"></div>
            </SwitchBar.Switch>
          </SwitchBar>
        </div>
        <div className="side-content">
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
    <>
      <div className="header">
        <ActionBar />
      </div>
      <div className="content">
        <div className="pane full">
          <FileExplorer root={rootList} />
        </div>
      </div>
    </>
  );
}


function TwoPaneView (props) {
  const { rootList } = props;
  return (
    <>
      <div className="header">
        <ActionBar />
      </div>
      <div className="content">
        <div className="pane half">
          <FileExplorer root={rootList} />
        </div>
        <div className="pane half">
          <FileExplorer root={rootList} />
        </div>
      </div>
    </>
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
