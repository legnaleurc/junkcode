import React from 'react';
import { connect } from 'react-redux';

import { getImageUrl } from '../states/file_system/actions';

import './image_view.scss';


class ImageView extends React.Component {

  constructor (props) {
    super(props);

    this.state = {
      url: '',
    };

    this._root = React.createRef();
    const options = {
      root: null,
      rootMargin: '0px',
      threshold: buildThresholdList(),
    };
    this._observer = new IntersectionObserver(this._handleIntersect.bind(this), options);
    this._prevRatio = 0.0;
    this._increasingColor = 'rgba(40, 40, 190, ratio)';
    this._decreasingColor = 'rgba(190, 40, 40, ratio)';

    this._setImageUrl = this._setImageUrl.bind(this);
  }

  componentDidMount () {
    // this._observer.observe(this._root.current);
    const { getImageUrl, nodeId, index } = this.props;
    getImageUrl(nodeId, index, this._setImageUrl);
  }

  componentWillUnmount () {
    // this._observer.disconnect();
  }

  render () {
    const { width, height } = this.props;
    return (
      <div
        className="image-view"
        style={{
          width: `${width}px`,
          height: `${height}px`,
        }}
        ref={this._root}
      >
        <img src={this.state.url} />
      </div>
    );
  }

  _setImageUrl (url) {
    this.setState({
      url,
    });
  }

  _handleIntersect (entries) {
    entries.forEach((entry) => {
      if (entry.intersectionRatio > this._prevRatio) {
        entry.target.style.backgroundColor = this._increasingColor.replace("ratio", entry.intersectionRatio);
      } else {
        entry.target.style.backgroundColor = this._decreasingColor.replace("ratio", entry.intersectionRatio);
      }

      this._prevRatio = entry.intersectionRatio;
    });
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
