import React from 'react';

import './image_view.scss';


class ImageView extends React.Component {

  constructor (props) {
    super(props);

    this._root = React.createRef();
  }

  componentDidMount () {
    console.info('componentDidMount', this._root);
  }

  componentWillUnmount () {
    console.info('componentWillUnmount', this._root);
  }

  render () {
    return (
      <div className="image-view" ref={this._root}></div>
    );
  }

}


export default ImageView;
