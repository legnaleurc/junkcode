import React from 'react';
import { connect } from 'react-redux';

import { getStreamUrl } from '../states/file_system/actions';
import { getSearchName } from '../states/search/actions';
import { selectMatchedList, clearSelection } from '../states/selection/actions';
import Input from './input';
import Selectable from './selectable';
import ActionBar from './action_bar';

import './search_list.scss';


class SearchList extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      loading: false,
    };

    this._search = this._search.bind(this);
  }

  render () {
    const { matched, history, selectMatchedList } = this.props;
    return (
      <div className="search-list">
        <div className="input-group">
          <Input
            type="text"
            onKeyPress={event => {
              if (event.key !== 'Enter') {
                return;
              }
              event.preventDefault();
              this._search(event.target.value);
            }}
          />
          <ActionBar />
        </div>
        <div>
          <HistoryList history={history} search={this._search} />
        </div>
        <div>
          {this._renderEmpty()}
          {matched.map(({id, path}) => (
            <div
              key={id}
              onDoubleClick={event => {
                event.preventDefault();
                this._openFile(id);
              }}
            >
              <Selectable.Area nodeId={id}>
                <Selectable.Trigger
                  nodeId={id}
                  onMultiSelect={selectMatchedList}
                >
                  <code>{path}</code>
                </Selectable.Trigger>
              </Selectable.Area>
            </div>
          ))}
        </div>
      </div>
    );
  }

  _renderEmpty () {
    const { matched } = this.props;
    if (!matched || matched.length <= 0) {
      return <EmptyBlock />;
    }
    return null;
  }

  _search (text) {
    const { clearSelection, searchName } = this.props;
    clearSelection();
    searchName(text);
  }

  _openFile (nodeId) {
    const { getFileUrl } = this.props;
    getFileUrl(nodeId, openUrl);
  }

};


function EmptyBlock (props) {
  return (
    <div className="empty-block">EMPTY</div>
  );
}


function HistoryList (props) {
  return (
    <div className="history-list">
      {props.history.map((name, i) => (
        <HistoryEntry
          key={i}
          name={name}
          search={props.search}
        />
      ))}
    </div>
  );
}


function HistoryEntry (props) {
  return (
    <pre
      className="history-entry"
      onClick={event => {
        props.search(props.name);
      }}
    >
      {props.name}
    </pre>
  );
}


function openUrl (url) {
  function onCopy (event) {
    event.preventDefault();
    event.clipboardData.setData('text/plain', url);
    document.removeEventListener('copy', onCopy);
  }
  document.addEventListener('copy', onCopy);
  document.execCommand('copy');
}


function mapStateToProps (state) {
  const { search } = state;
  return {
    matched: search.matched,
    history: search.history,
  };
}


function mapDispatchToProps (dispatch) {
  return {
    searchName (name) {
      dispatch(getSearchName(name));
    },
    getFileUrl (id, done) {
      dispatch(getStreamUrl(id, done));
    },
    selectMatchedList (id) {
      dispatch(selectMatchedList(id));
    },
    clearSelection () {
      dispatch(clearSelection());
    },
  };
}


export default connect(mapStateToProps, mapDispatchToProps)(SearchList);
