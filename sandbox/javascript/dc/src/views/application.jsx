import React from 'react';
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

    this.state = {
      selected: 'normal',
    };
  }

  render () {
    const { clearSelection } = this.props;
    return (
      <div className="application">
        <div className="side-bar">
          <SwitchBar
            selected={this.state.selected}
            onSwitch={(key) => {
              this.setState({
                selected: key,
              });
              clearSelection();
            }}
          >
            <SwitchBar.Switch name="normal">
              <div className="block normal"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="search">
              <div className="block search"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="two-pane">
              <div className="block two-pane"></div>
            </SwitchBar.Switch>
            <SwitchBar.Switch name="mpv">
              <div className="block mpv"></div>
            </SwitchBar.Switch>
          </SwitchBar>
        </div>
        <div className="side-content">
          <MutexView selected={this.state.selected}>
            <MutexView.Mutex name="normal">
              <SingleTreeView />
            </MutexView.Mutex>
            <MutexView.Mutex name="search">
              <SearchList />
            </MutexView.Mutex>
            <MutexView.Mutex name="two-pane">
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


const ConnectedApplication = connect(undefined, mapDispatchToProps)(Application);
export default hot(ConnectedApplication);
