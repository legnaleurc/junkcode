import {Event} from './event.js';
import * as parser from './parser.js';


class Model extends Event {

  constructor (data) {
    super()

    this._data = data;
  }

  get (key) {
    return this._data[key];
  }

  set (key, value) {
    this._data[key] = value;
    return this;
  }

}


class Collection extends Event {

  constructor () {
    super()

    this._list = [];
  }

  forEach (fn, context) {
    this._list.forEach(fn, context || this);
    return this;
  }

  map (fn, context) {
    return this._list.map(fn, context || this);
  }

  reset (models) {
    if (typeof models === 'undefined') {
      this._list = [];
      return this;
    }

    this._list = models.map(function (model) {
      if (model instanceof Model) {
        return model;
      }
      var ModelClass = this.model;
      return new ModelClass(model);
    }.bind(this));

    return this;
  }

}


class ComicModel extends Model {

  fetch () {
    var url = this.get('url');
    return parser.getSummary(url).then(function (comic) {
      this.set('coverURL', comic.coverURL);
      return this;
    }.bind(this));
  }

}


export class LatestUpdateCollection extends Collection {

  get model () {
    return ComicModel;
  }

  fetch () {
    return parser.getLatest().then(function (comicList) {
      this.reset(comicList);
      return this;
    }.bind(this));
  }

}
