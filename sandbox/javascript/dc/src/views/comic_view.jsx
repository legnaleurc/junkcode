import React from 'react';

import ImageView from './image_view';


class ComicView extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    return (
      <div className="comic-view">
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
        <ImageView />
      </div>
    );
  }

}


export default ComicView;
