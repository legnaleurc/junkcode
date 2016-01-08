import * as view from './view.js';
import * as model from './model.js';
import {View} from './core.js';


document.addEventListener('DOMContentLoaded', function() {
  var latestUpdateCollection = new model.LatestUpdateCollection();

  var navigationView = new view.NavigationView({
    el: '#nav-bar',
  });

  var latestUpdateView = new view.LatestUpdateView({
    el: '#latest-update',
    model: latestUpdateCollection,
  });

  var comicView = new view.ComicView({
    el: '#comic-info',
    model: new model.ComicModel({
      id: '0',
    }),
  });

  var episodeView = new view.EpisodeView({
    el: '#episode',
    model: new model.PageModel(),
  });

  View.router.add('index', latestUpdateView);
  View.router.add('comic', comicView);
  View.router.add('episode', episodeView);
  View.router.start('index');
});
