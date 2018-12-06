import React from 'react';
import { connect } from 'react-redux';

import ImageView from './image_view';


class ComicView extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    return (
      <div className="comic-view">
        {this.props.imageList.map((d, index) => (
          <React.Fragment key={index}>
            <ImageView index={index} width={d.width} height={d.height} />
          </React.Fragment>
        ))}
      </div>
    );
  }

}


function mapStateToProps (state) {
  const { imageList } = state.selection;
  return {
    imageList,
  };
}


export default connect(mapStateToProps)(ComicView);
