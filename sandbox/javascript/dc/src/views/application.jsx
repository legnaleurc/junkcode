import React from 'react';
import { BrowserRouter, Switch, Route, Redirect } from "react-router-dom";
import { connect } from 'react-redux';
import { hot } from 'react-hot-loader/root';

import SearchList from './search_list';
import MultiPageView from './multipage_view';
import SwitchBar from './switch_bar';
import MutexView from './mutex_view';
import SingleTreeView from './single_tree_view';
import DoubleTreeView from './double_tree_view';
import { clearSelection } from '../states/selection/actions';

import './application.scss';


class Application extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { clearSelection, match, history } = this.props;
    return (
      <div className="application">
        <div className="side-bar">
          <SwitchBar
            selected={match.params.tabId}
            onSwitch={(key) => {
              clearSelection();
              history.push(`/${key}`);
            }}
          >
            <SwitchBar.Switch name="tree">
              <div className="block normal"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="search">
              <div className="block search"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="double-tree">
              <div className="block two-pane"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="mpv">
              <div className="block mpv"></div>
            </SwitchBar.Switch>
          </SwitchBar>
        </div>
        <div className="side-content">
          <MutexView selected={match.params.tabId}>
            <MutexView.Mutex name="tree">
              <SingleTreeView />
            </MutexView.Mutex>
            <MutexView.Mutex name="search">
              <SearchList />
            </MutexView.Mutex>
            <MutexView.Mutex name="double-tree">
              <DoubleTreeView />
            </MutexView.Mutex>
            <MutexView.Mutex name="mpv">
              <MultiPageView />
            </MutexView.Mutex>
          </MutexView>
        </div>
      </div>
    );
  }

}


function mapDispatchToProps (dispatch) {
  return {
    clearSelection () {
      dispatch(clearSelection());
    },
  };
}


const ConnectedApplication = connect(
  undefined,
  mapDispatchToProps,
)(Application);


class RoutedApplication extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    return (
      <BrowserRouter>
        <Switch>
          <Redirect exact from="/" to="/tree" />
          <Route
            path="/:tabId"
            component={ConnectedApplication}
          />
        </Switch>
      </BrowserRouter>
    );
  }

}


export default hot(RoutedApplication);
