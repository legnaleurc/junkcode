import * as view from './view.js';
import * as model from './model.js';
import {router} from './core.js';


router.on('change', function (event) {
  console.info('change', event.detail);
});


document.addEventListener('DOMContentLoaded', function() {
  var a = new view.LatestUpdateView({
    el: '#latest-update',
    model: new model.LatestUpdateCollection(),
  });
  a.render().catch(function (e) {
    console.warn(e);
  });
});
