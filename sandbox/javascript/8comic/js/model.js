define('model', ['core', 'parser'], function (core, parser) {
  'use strict';
  
  function Model (data) {
    this._data = data;
  }
  Model.extend = core.extend;
  
  Model.prototype.get = function (key) {
    return this._data[key];
  };
  
  Model.prototype.set = function (key, value) {
    this._data[key] = value;
    return this;
  };
  
  
  function Collection (data_list) {
    this._list = data_list;
  }
  Collection.extend = core.extend;
  
  Collection.prototype.reset = function (models) {
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
  };
  

  var ComicModel = Model.extend({
    
    fetch: function () {
      var url = this.get('url');
      return parser.getSummary(url).then(function (comic) {
        this.set('coverURL', data.coverURL);
      }.bind(this));
    },
    
  });
  
  
  var LatestUpdateModel = Collection.extend({
    
    model: ComicModel,
    
    fetch: function () {
      return parser.getLatest().then(function (comicList) {
        this.reset(comicList);
      }.bind(this));
    },
    
  });
  
});
