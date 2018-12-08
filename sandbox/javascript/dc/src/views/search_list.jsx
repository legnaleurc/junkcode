import React from 'react';
import { connect } from 'react-redux';

import { getSearchName, getStreamUrl } from '../states/file_system/actions';
import { selectMatchedList } from '../states/selection/actions';
import Input from './input';
import Selectable from './selectable';


class SearchList extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      loading: false,
    };
  }

  render () {
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
        </div>
        <div>
          {this._renderEmpty()}
          {this.props.matched.map(({id, path}) => (
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
    this.props.searchName(text);
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
  const { fileSystem } = state;
  return {
    matched: fileSystem.matched,
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
  };
}


export default connect(mapStateToProps, mapDispatchToProps)(SearchList);
