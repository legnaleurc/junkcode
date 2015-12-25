require(['parser', 'view'], function (parser, view) {
  'use strict';
  
  document.addEventListener('DOMContentLoaded', function() {
    parser.getLatest().then(function (__) {
      __.map(view.createEntry);
    });
  });
});
