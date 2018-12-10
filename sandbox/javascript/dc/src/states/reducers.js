import { combineReducers } from 'redux';

import reduceFileSystem from './file_system/reducers';
import reduceMultiPage from './multipage/reducers';
import reduceSearch from './search/reducers';
import reduceSelection from './selection/reducers';


export default combineReducers({
  fileSystem: reduceFileSystem,
  mpv: reduceMultiPage,
  search: reduceSearch,
  selection: reduceSelection,
});
