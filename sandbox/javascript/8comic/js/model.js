import {Model, Collection} from './core.js';
import * as parser from './parser.js';


export class ComicModel extends Model {

  static get (id) {
    return globalComicCollection.get(id);
  }

  constructor (args) {
    args.url = args.url || '';
    var id = args.id || args.url.match(/\/(\d+)\.html$/);
    args.id = args.id || (id ? id[1] : '0');
    args.coverURL = args.coverURL || '';
    args.episodeList = args.episodeList || [];
    super(args);
  }

  fetch () {
    var url = this.get('url');
    return parser.getSummary(url).then(function (comic) {
      this.set('coverURL', comic.coverURL);
      this.set('episodeList', comic.episodeList);
      return this;
    }.bind(this));
  }

}


class ComicCollection extends Collection {

  get model () {
    return ComicModel;
  }

}
let globalComicCollection = new ComicCollection();


export class LatestUpdateCollection extends Collection {

  get model () {
    return ComicModel;
  }

  fetch () {
    return parser.getLatest().then(function (comicList) {
      comicList.forEach(function (comic) {
        globalComicCollection.add(comic);
      });
      this.reset(comicList);
      return this;
    }.bind(this));
  }

}


export class PageModel extends Model {

  // TODO 8comic specific, generalize this
  fetch () {
    var payload = this.get('payload');
    if (!payload || !payload.url || !payload.catid) {
      this.set('pageList', null);
      return Promise.resolve(this);
    }
    return parser.getPagesInEpisode(payload.url, payload.catid).then(function (pageList) {
      this.set('pageList', pageList);
      return this;
    }.bind(this));
  }
}
