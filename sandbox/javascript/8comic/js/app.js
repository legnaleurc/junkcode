(function (exports) {
  'use strict';

  var parser = exports.parser;
  var view = exports.view;

  document.addEventListener('DOMContentLoaded', function() {
    parser.getLatest().then(function (__) {
      __.map(view.createEntry);
    });
  });
  
})(window);
