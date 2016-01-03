import {View, router} from './core.js';


export class LatestUpdateView extends View {

  constructor (args) {
    super(args);

    this._summaries = [];
  }

  render () {
    if (window.__test__) {
      debugger;
    } else {
      window.__test__ = '__MAGIC__';
    }
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

      this.el.addEventListener('click', function (event) {
        var id = this.model.get('id');
        router.push({
          type: 'comic',
          id: id,
        }, `#comic/${id}`);
      }.bind(this));
    }.bind(this)).then(function () {
      return this.model.fetch();
    }.bind(this));
  }

  _onCoverURLChange (event) {
    var url = this.model.get('coverURL');
    this.el.style.backgroundImage = `url("${url}")`;
  }

}
