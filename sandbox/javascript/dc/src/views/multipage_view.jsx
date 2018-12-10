import React from 'react';
import { connect } from 'react-redux';

import ImageView from './image_view';

import './multipage_view.scss';


class MultiPageView extends React.Component {

  constructor (props) {
    super(props);

    this._root = React.createRef();
  }

  render () {
    return (
      <div className="multipage-view" ref={this._root}>
        {this.props.imageList.map((d, index) => (
          <React.Fragment key={index}>
            <ImageView
              rootRef={this._root}
              width={d.width}
              height={d.height}
              url={d.url}
            />
          </React.Fragment>
        ))}
      </div>
    );
  }

}


function mapStateToProps (state) {
  const { imageList } = state.mpv;
  return {
    imageList,
  };
}


export default connect(mapStateToProps)(MultiPageView);
