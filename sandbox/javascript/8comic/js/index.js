require(['parser', 'model', 'view'], function (parser, model, view) {
  'use strict';
  
  document.addEventListener('DOMContentLoaded', function() {
    var a = new view.LatestUpdateView({
      el: '#content',
      model: new model.LatestUpdateModel(),
    });
    a.render();
  });
});
