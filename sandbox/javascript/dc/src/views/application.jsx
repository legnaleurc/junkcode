import React from 'react';
import { connect } from 'react-redux';
import { hot } from 'react-hot-loader';

import ActionBar from './action_bar';
import FileExplorer from './file_explorer';
import SearchList from './search_list';
import MultiPageView from './multipage_view';
import SwitchBar from './switch_bar';
import MutexView from './mutex_view';
import { getRoot } from '../states/file_system/actions';
import { clearSelection } from '../states/selection/actions';

import './application.scss';


class Application extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      selected: 'normal',
    };
  }

  componentDidMount() {
    const { getRoot } = this.props;

    getRoot();
  }

  render () {
    const { rootId, clearSelection } = this.props;
    return (
      <div className="application">
        <div className="side-bar">
          <SwitchBar onSwitch={(key) => {
            this.setState({
              selected: key,
            });
            clearSelection();
          }}>
            <SwitchBar.Switch name="normal" default>
              <div className="switch normal"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="search">
              <div className="switch search"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="two-pane">
              <div className="switch two-pane"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="comic">
              <div className="switch comic"></div>
            </SwitchBar.Switch>
          </SwitchBar>
        </div>
        <div className="side-content">
          <MutexView selected={this.state.selected}>
            <MutexView.Mutex name="normal">
              <NormalView rootId={rootId} />
            </MutexView.Mutex>
            <MutexView.Mutex name="search">
              <SearchList />
            </MutexView.Mutex>
            <MutexView.Mutex name="two-pane">
              <TwoPaneView rootId={rootId} />
            </MutexView.Mutex>
            <MutexView.Mutex name="comic">
              <MultiPageView />
            </MutexView.Mutex>
          </MutexView>
        </div>
      </div>
    );
  }

}


function NormalView (props) {
  const { rootId } = props;
  return (
    <>
      <div className="header">
        <ActionBar />
      </div>
      <div className="content">
        <div className="pane full">
          <FileExplorer rootId={rootId} />
        </div>
      </div>
    </>
  );
}


function TwoPaneView (props) {
  const { rootId } = props;
  return (
    <>
      <div className="header">
        <ActionBar />
      </div>
      <div className="content">
        <div className="pane half">
          <FileExplorer rootId={rootId} />
        </div>
        <div className="pane half">
          <FileExplorer rootId={rootId} />
        </div>
      </div>
    </>
  );
}


function mapStateToProps (state) {
  return {
    rootId: state.fileSystem.rootId,
  };
}


function mapDispatchToProps (dispatch) {
  return {
    getRoot () {
      dispatch(getRoot());
    },
    clearSelection () {
      dispatch(clearSelection());
    },
  };
}


export default hot(module)(connect(mapStateToProps, mapDispatchToProps)(Application));
