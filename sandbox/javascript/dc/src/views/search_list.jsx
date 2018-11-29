import React from 'react';
import { connect } from 'react-redux';

import { getSearchName, getStreamUrl } from '../states/file_system/actions';


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
        <input type="text" onKeyPress={event => {
          if (event.key !== 'Enter') {
            return;
          }
          event.preventDefault();
          this._search(event.target.value);
        }} />
        <div>
          {this.props.matched.map(({id, path}) => (
            <p
              key={id}
              onDoubleClick={event => {
                event.preventDefault();
                this._openFile(id);
              }}
            >
              <code>{path}</code>
            </p>
          ))}
        </div>
      </div>
    );
  }

  _search (text) {
    this.props.searchName(text);
  }

  _openFile (nodeId) {
    const { getFileUrl } = this.props;
    getFileUrl(nodeId, openUrl);
  }

};


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
  };
}


export default connect(mapStateToProps, mapDispatchToProps)(SearchList);
