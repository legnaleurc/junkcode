import { combineReducers } from 'redux';

import reduceFileSystem from './file_system/reducers';


export default combineReducers({
  fileSystem: reduceFileSystem,
});
