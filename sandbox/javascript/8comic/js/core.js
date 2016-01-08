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

    this._data = data || {};
  }

  get (key) {
    return this._data[key];
  }

  set (key, value) {
    if (key instanceof Model) {
      for (let k in key._data) {
        if (key._data.hasOwnProperty(k)) {
          this.set(k, key._data[k]);
        }
      }
      return this;
    }
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

  // TODO update duplicate
  add (model) {
    if (model instanceof Model) {
      this._dict[model.get('id')] = model;
      return model;
    }
    var ModelClass = this.model;
    model = new ModelClass(model);
    this._dict[model.get('id')] = model;
    this._list.push(model);
    return this;
  }

  reset (models) {
    this._list = [];
    this._dict = {};

    if (typeof models === 'undefined') {
      return this;
    }

    models.forEach(function (model) {
      this.add(model);
    }.bind(this));

    return this;
  }

}


class Router extends Event {

  constructor () {
    super();

    this._activities = {};
    this._stack = [];
  }

  start (initialType, args) {
    var view = this._activities[initialType];
    return view.render().then(function () {
      return view.show();
    }).then(function () {
      this._stack.push(initialType);
      this.trigger('change', {
        type: initialType,
        args: args,
      });
    }.bind(this));
  }

  add (type, view) {
    this._activities[type] = view;
  }

  push (type, args) {
    for (let t in this._activities) {
      if (this._activities.hasOwnProperty(t)) {
        if (t === type) {
          continue;
        }
        this._activities[t].hide();
      }
    }
    var view = this._activities[type];
    return view.render().then(function () {
      return view.show();
    }).then(function () {
      this._stack.push(type);
      this.trigger('change', {
        type: type,
        args: args,
      });
      return this;
    }.bind(this));
  }

  pop () {
    var latestType = this._stack.pop();
    var index = this._stack.length - 1;
    if (index < 0) {
      return;
    }

    var topType = this._stack[index];
    for (let t in this._activities) {
      if (this._activities.hasOwnProperty(t)) {
        if (t === topType) {
          continue;
        }
        this._activities[t].hide();
      }
    }
    var view = this._activities[topType];
    return view.render().then(function () {
      return view.show();
    });
  }

}
let globalRouter = new Router();


export class View extends Event {

  static get router () {
    return globalRouter;
  }

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
    return Promise.resolve(this);
  }

  show () {
    this.el.classList.add('active');
    return Promise.resolve(this);
  }

  hide () {
    this.el.classList.remove('active');
    return Promise.resolve(this);
  }

}
