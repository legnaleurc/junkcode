import React from 'react';

import { classNameFromObject } from '../lib/index';

import './image_view.scss';


const DUMMY_IMAGE_URL = 'data:image/gif;base64,R0lGODlhAQABAAAAACH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==';


class ImageView extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      url: DUMMY_IMAGE_URL,
      loaded: false,
    };

    this._handleIntersect = this._handleIntersect.bind(this);
    this._setImageUrl = this._setImageUrl.bind(this);

    this._root = React.createRef();
    const options = {
      root: props.rootRef.current,
      rootMargin: '400% 0px 400% 0px',
      threshold: 0,
    };
    this._observer = new IntersectionObserver(this._handleIntersect, options);
  }

  componentDidMount () {
    this._observer.observe(this._root.current);
  }

  componentWillUnmount () {
    this._observer.disconnect();
  }

  render () {
    const { width, height } = this.props;
    const { url, loaded } = this.state;
    return (
      <div
        className="image-view"
        ref={this._root}
      >
        <img
          className={classNameFromObject({
            image: true,
            loaded,
          })}
          src={url}
          width={width}
          height={height}
        />
      </div>
    );
  }

  _setImageUrl (url) {
    this.setState({
      url,
      loaded: true,
    });
  }

  _handleIntersect (entries) {
    entries.forEach(entry => {
      if (entry.isIntersecting) {
        this._setImageUrl(this.props.url);
      }
    });
  }

}

export default ImageView;
