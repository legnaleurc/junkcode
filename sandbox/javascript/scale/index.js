(function () {
  'use strict';

  var el = document.querySelector('#content');
  var looping = false;
  var originalPinches = {};
  var scale = 1.0;

  function startLoop () {
    looping = true;
    loop();
  }

  function stopLoop () {
    looping = false;
  }

  function loop () {
    requestAnimationFrame(function () {
      if (!looping) {
        return;
      }
      el.style.transform = `scale(${scale})`;
      loop();
    });
  }

  function calculateScale (touches) {
    var pinch = Object.assign({}, originalPinches);
    Array.forEach(touches, (touch) => {
      pinch[touch.identifier] = {
        x: touch.clientX,
        y: touch.clientY,
      };
    });
    var a = this._abs(originalPinches);
    var b = this._abs(pinch);
    var c = b / a;
    c = c <= 1.0 ? 1.0 : c;
    return c;
  }

  el.addEventListener('touchstart', function (event) {
    Array.forEach(event.touches, (touch) => {
      originalPinches[touch.identifier] = {
        x: touch.clientX,
        y: touch.clientY,
      };
    });

    startLoop();
  });

  el.addEventListener('touchend', function (event) {
    Array.forEach(event.touches, (touch) => {
      delete originalPinches[touch.identifier];
    });

    stopLoop();
  });

  el.addEventListener('touchmove', function (event) {
    if (Object.keys(originalPinches).length !== 2) {
      return;
    }

    scale = calculateScale(event.touches);
  });

})();
