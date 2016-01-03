export class Event {

  constructor () {
    this._listenable = document.createDocumentFragment();
  }

  on (type, callback) {
    this._listenable.addEventListener(type, callback);
    return this;
  }

  off (type) {
    return this;
  }

  trigger (type, data) {
    var event = new CustomEvent(type, {
      detail: data,
    });
    this._listenable.dispatchEvent(event);
    return this;
  }

}


export class Model extends Event {

  constructor (data) {
    super()

    this._data = data;
  }

  get (key) {
    return this._data[key];
  }

  set (key, value) {
    var oldValue = this._data[key];
    this._data[key] = value;
    this.trigger(`change:${key}`, {
      originalValue: oldValue,
    });
    return this;
  }

}


export class Collection extends Event {

  constructor () {
    super()

    this._list = [];
    this._dict = {};
  }

  get (id) {
    return this._dict[id];
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
      this._dict = {};
      return this;
    }

    this._dict = {};
    this._list = models.map(function (model) {
      if (model instanceof Model) {
        this._dict[model.get('id')] = model;
        return model;
      }
      var ModelClass = this.model;
      model = new ModelClass(model);
      this._dict[model.get('id')] = model;
      return model;
    }.bind(this));

    return this;
  }

}


export class View extends Event {

  constructor (args) {
    super();
    args = args || {};

    this._model = args.model || null;

    if (typeof args.el === 'string') {
      this._el = document.querySelector(args.el);
    } else if (args.el instanceof Element) {
      this._el = args.el;
    } else {
      this._el = null;
    }

    this._tagName = args.tagName || 'div';
  }

  get model () {
    return this._model;
  }

  set model (value) {
    this._model = value;
    return true;
  }

  get el () {
    return this._el;
  }

  render () {
    if (!this._el) {
      this._el = document.createElement(this._tagName);
    }
    this.el.style.display = '';
    return Promise.resolve(this);
  }

  hide () {
    this.el.style.display = 'none';
    return this;
  }

}


class Router extends Event {

  push (state, hash) {
    history.pushState(state, hash, hash);
    this.trigger('change', state);
  }

}
export var router = new Router();
window.addEventListener('popstate', function (event) {
  router.trigger('change', event.state);
});
