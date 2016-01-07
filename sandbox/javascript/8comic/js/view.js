import {View} from './core.js';
import {ComicModel} from './model.js';


export class LatestUpdateView extends View {

  constructor (args) {
    super(args);

    this._summaries = [];
    this._initialized = false;
  }

  render () {
    // TODO extract to super
    if (this._initialized) {
      return Promise.resolve(this);
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
      }.bind(this));

      this._initialized = true;
      return this;
    }.bind(this));
  }

}


class SummaryView extends View {

  constructor (args) {
    super(args);

    this._initialized = false;

    this.model.on('change:coverURL', this._onCoverURLChange.bind(this));
  }

  render () {
    // TODO extract to super
    if (this._initialized) {
      return Promise.resolve(this);
    }

    return super.render().then(function (self) {
      this.el.classList.add('comic-entry');
      var title = document.createElement('div');
      title.classList.add('title');
      title.textContent = this.model.get('title');
      this.el.appendChild(title);

      this.el.addEventListener('click', function (event) {
        var id = this.model.get('id');
        View.router.push('comic', {
          id: id,
        }, `#comic/${id}`);
      }.bind(this));

      this._initialized = true;
    }.bind(this)).then(function () {
      return this.model.fetch();
    }.bind(this));
  }

  _onCoverURLChange (event) {
    var url = this.model.get('coverURL');
    this.el.style.backgroundImage = `url("${url}")`;
  }

}


export class ComicView extends View {

  constructor (args) {
    super(args);

    this._initialized = false;

    var episodeList = document.querySelector('#episode-list');
    episodeList.addEventListener('click', function (event) {
      var li = event.target;
      var url = li.dataset.url;
      var catid = li.dataset.catid;
      if (!url || !catid) {
        return;
      }

      View.router.push('episode', {
        url: url,
        catid: catid,
      });
    });

    this.model.on('change:title', function () {
      var title = document.querySelector('#title');
      title.textContent = this.model.get('title');
    }.bind(this));

    this.model.on('change:coverURL', function () {
      var cover = document.querySelector('#cover');
      cover.src = this.model.get('coverURL');
    }.bind(this));

    this.model.on('change:episodeList', function () {
      while (episodeList.firstChild) {
        episodeList.removeChild(episodeList.firstChild);
      }

      this.model.get('episodeList').forEach(function (episode) {
        var li = document.createElement('li');
        li.textContent = episode.name;
        li.dataset.url = episode.url;
        li.dataset.catid = episode.catid;
        episodeList.appendChild(li);
      });
    }.bind(this));

    // TODO simplify this
    View.router.on('change', function (event) {
      var detail = event.detail;
      if (detail.type !== 'comic') {
        return;
      }
      var model = ComicModel.get(detail.args.id);
      this.model.set(model);
    }.bind(this));
  }

  render () {
    // TODO extract to super
    if (this._initialized) {
      return Promise.resolve(this);
    }

    return super.render().then(function () {
      this._initialized = true;
    }.bind(this));
  }

}


export class EpisodeView extends View {

  constructor (args) {
    super(args);

    // TODO simplify this
    View.router.on('change', function (event) {
      var detail = event.detail;
      if (detail.type !== 'episode') {
        return;
      }
      this.model.set('payload', detail.args);
    }.bind(this));

    this.model.on('change:payload', function (event) {
      this.model.fetch().then(function () {
        var pageList = this.model.get('pageList');
        if (!pageList) {
          return this;
        }
        pageList.forEach(function (pageURL) {
          var img = document.createElement('img');
          img.src = pageURL;
          this.el.appendChild(img);
        }.bind(this));
      }.bind(this));
    }.bind(this));
  }

  render () {
    return super.render().then(function () {
      while (this.el.firstChild) {
        this.el.removeChild(this.el.firstChild);
      }
      return this;
    }.bind(this));
  }

}
