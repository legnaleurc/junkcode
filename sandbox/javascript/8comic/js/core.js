define('core', [], function () {
  'use strict';
  
  var slice = Array.prototype.slice;
  
  function extend (object) {
    slice.call(arguments, 1).forEach(function (source) {
      var prop;
      if (!source) {
        return;
      }
      for (prop in source) {
        if (source.hasOwnProperty(prop)) {
          object[prop] = source[prop];
        }
      }
    });
    return object;
  }
  
  return {
    extend: function (protoProps, staticProps) {
      var parent = this, child;

      if (protoProps && protoProps.hasOwnProperty('constructor')) {
        child = protoProps.constructor;
      } else {
        child = function () {
          return parent.apply(this, arguments);
        };
      }

      extend(child, parent, staticProps);

      child.prototype = Object.create(parent.prototype);
      child.prototype.constructor = child;

      if (protoProps) {
        extend(child.prototype, protoProps);
      }

      child.super = parent.prototype;

      return child;
    },
  };
  
});
