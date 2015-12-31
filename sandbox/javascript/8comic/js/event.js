define('event', ['core'], function (core) {
  'use strict';
  
  class Event {
    
    constructor () {
      this._listenable = document.createDocumentFragment();
    }
    
    on (type, callback) {
      this._listenable.addEventListener(type, callback);
      return this;
    }
  
    off (type) {
      return this;
    }
  
    trigger (type, data) {
      var event = new CustomEvent(type, data);
      this._listenable.dispatchEvent(event);
      return this;
    }
    
  }

  return {
    Event: Event,
  };
  
});
