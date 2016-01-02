import {Event} from './event.js';


class View extends Event {

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

    this._tagName = args.tagName;
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
      if (!this._tagName) {
        return null;
      }
      this._el = document.createElement(this._tagName);
    }
    return Promise.resolve(this);
  }

}


export class LatestUpdateView extends View {

  constructor (args) {
    super(args);

    this._summaries = [];
  }

  render () {
    return super.render().then(function (self) {
      return self.model.fetch();
    }).then(function () {
      this._summaries = this.model.map(function (comicModel) {
        var view = new SummaryView({
          model: comicModel,
        });
        view.render().then(function () {
          this.el.appendChild(view.el);
        }.bind(this));
        return view;
      }.bind(this))
      return this;
    }.bind(this));
  }

}


class SummaryView extends View {

  constructor (args) {
    args.tagName = 'div';
    super(args);

    this.model.on('change:coverURL', this._onCoverURLChange.bind(this));
  }

  render () {
    return super.render().then(function (self) {
      this.el.classList.add('comic-entry');
      var title = document.createElement('div');
      title.classList.add('title');
      title.textContent = this.model.get('title');
      this.el.appendChild(title);
    }.bind(this)).then(function () {
      return this.model.fetch();
    }.bind(this));
  }

  _onCoverURLChange (event) {
    var url = this.model.get('coverURL');
    this.el.style.backgroundImage = `url("${url}")`;
  }

}
