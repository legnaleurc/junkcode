import * as view from './view.js';
import * as model from './model.js';
import {View} from './core.js';


document.addEventListener('DOMContentLoaded', function() {
  var latestUpdateCollection = new model.LatestUpdateCollection();

  var latestUpdateView = new view.LatestUpdateView({
    el: '#latest-update',
    model: latestUpdateCollection,
  });
  latestUpdateView.render().catch(function (e) {
    console.warn(e);
  });

  var comicView = new view.ComicView({
    el: '#comic-info',
    model: new model.ComicModel({
      id: '0',
    }),
  });

  View.router.add('index', latestUpdateView);
  View.router.add('comic', comicView);
  View.router.start('index');
});
