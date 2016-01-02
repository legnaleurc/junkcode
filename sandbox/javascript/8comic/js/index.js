import * as view from './view.js';
import * as model from './model.js';


document.addEventListener('DOMContentLoaded', function() {
  var a = new view.LatestUpdateView({
    el: '#content',
    model: new model.LatestUpdateCollection(),
  });
  a.render().catch(function (e) {
    console.warn(e);
  });
});
