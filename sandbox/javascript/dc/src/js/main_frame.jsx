import React from 'react';
import { hot } from 'react-hot-loader';

import FileExplorer from './file_explorer';


class MainFrame extends React.Component {

  constructor (props) {
    super(props);
  }

  render () {
    const { fileSystem, root } = this.props;
    return (
      <div>
        <FileExplorer fileSystem={fileSystem} root={root} />
      </div>
    );
  }

}


export default hot(module)(MainFrame);
