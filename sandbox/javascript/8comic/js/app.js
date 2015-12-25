require(['parser', 'view'], function (parser, view) {
  'use strict';
  
  document.addEventListener('DOMContentLoaded', function() {
    var a = new view.LatestUpdateView();
    a.show();
  });
});
