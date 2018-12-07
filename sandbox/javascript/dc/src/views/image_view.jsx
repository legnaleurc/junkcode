import React from 'react';
import { connect } from 'react-redux';

import { getImageUrl } from '../states/file_system/actions';

import './image_view.scss';


const DUMMY_IMAGE_URL = 'data:image/gif;base64,R0lGODlhAQABAAAAACH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==';


class ImageView extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      url: DUMMY_IMAGE_URL,
    };

    this._handleIntersect = this._handleIntersect.bind(this);
    this._setImageUrl = this._setImageUrl.bind(this);

    this._root = React.createRef();
    const options = {
      root: null,
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
    return (
      <div
        className="image-view"
        ref={this._root}
      >
        <img
          className="image"
          src={this.state.url}
          width={width}
          height={height}
        />
      </div>
    );
  }

  _setImageUrl (url) {
    this.setState({
      url,
    });
  }

  _handleIntersect (entries) {
    const { getImageUrl, nodeId, index } = this.props;
    getImageUrl(nodeId, index, this._setImageUrl);
  }

}


function buildThresholdList () {
  const thresholds = [];
  const numSteps = 20;

  for (let i = 1.0; i <= numSteps; ++i) {
    const ratio = i / numSteps;
    thresholds.push(ratio);
  }

  thresholds.push(0);
  return thresholds;
}


function mapStateToProps (state) {
  const { viewingId } = state.selection;
  return {
    nodeId: viewingId,
  };
}


function mapDispatchToProps (dispatch) {
  return {
    getImageUrl (id, imageId, done) {
      return dispatch(getImageUrl(id, imageId, done));
    },
  };
}


export default connect(mapStateToProps, mapDispatchToProps)(ImageView);
