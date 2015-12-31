define('model', ['core', 'event', 'parser'], function (core, event, parser) {
  'use strict';

  class Model extends event.Event {

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


  class Collection extends event.Event {

    constructor () {
      super()

      this._list = [];
    }

    forEach (fn, context) {
      this._list.forEach(fn, context || this);
      return this;
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
        return this.model(model);
      }.bind(this));

      return this;
    }

  }


  class ComicModel extends Model {

    fetch () {
      var url = this.get('url');
      return parser.getSummary(url).then(function (comic) {
        this.set('coverURL', data.coverURL);
      }.bind(this));
    }

  }


  class LatestUpdateCollection extends Collection {

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

  return {
    ComicModel: ComicModel,
    LatestUpdateCollection: LatestUpdateCollection,
  };

});
