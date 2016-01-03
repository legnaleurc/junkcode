import {Model, Collection} from './core.js';
import * as parser from './parser.js';


class ComicModel extends Model {

  constructor (args) {
    var id = args.url.match(/\/(\d+)\.html$/);
    args.id = id ? id[1] : '0';
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
