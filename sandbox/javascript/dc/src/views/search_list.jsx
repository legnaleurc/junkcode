import React from 'react';
import { connect } from 'react-redux';


class SearchList extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      loading: false,
    };
  }

  render () {
    return (
      <div>
        <input type="text" onKeyPress={event => {
          if (event.key !== 'Enter') {
            return;
          }
          event.preventDefault();
          this._search(event.target.value);
        }} />
      </div>
    );
  }

  _search (text) {
    console.info('text', text);
  }

};


export default connect()(SearchList);
