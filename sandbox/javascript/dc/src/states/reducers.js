import { combineReducers } from 'redux';

import reduceFileSystem from './file_system/reducers';
import reduceSelection from './selection/reducers';


export default combineReducers({
  fileSystem: reduceFileSystem,
  selection: reduceSelection,
});
