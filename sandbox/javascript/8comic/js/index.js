import * as view from './view.js';
import * as model from './model.js';
import {router} from './core.js';


document.addEventListener('DOMContentLoaded', function() {
  var latestUpdateCollection = new model.LatestUpdateCollection();
  var latestUpdateView = new view.LatestUpdateView({
    el: '#latest-update',
    model: latestUpdateCollection,
  });
  latestUpdateView.render().catch(function (e) {
    console.warn(e);
  });

  var comicView = null;

  router.on('change', function (event) {
    var state = event.detail;
    if (state.type === 'index') {
      if (comicView) {
        comicView.hide();
      }
      latestUpdateView.render();
    } else if (state.type === 'comic') {
      latestUpdateView.hide();
      var comicModel = latestUpdateCollection.get(state.id);
      comicView = new view.ComicView({
        el: '#comic-info',
        model: comicModel,
      });
      comicView.render();
    } else {
      console.info(state);
    }
  });
});
