// ==UserScript==
// @name         GBF
// @namespace    foolproofproject
// @version      0.1
// @description  try to take over the world!
// @author       You
// @match        http://game.granbluefantasy.jp/*
// @grant        unsafeWindow
// ==/UserScript==

'use strict';


function factory ($) {
  var GBF = {
    ajaxActionList: [
      {
        path: /^\/deckcombination\/deck_combination_list\/(\d+)$/,
        action: function (args, request, response) {
          var metasetNumber = parseInt(args[1], 10);
          var sets = response.list;

          GBF.addMetaset(metasetNumber, sets);
        },
      },
      {
        path: /^\/weapon\/list\/\d+\/0$/,
        action: function (args, request, response) {
          var weapons = response.list;

          if (GBF.weaponLock) {
            GBF.weaponLock();
            GBF.weaponLock = null;
          }

//          GBF.addWeapons(weapons);
        },
      },
      {
        path: /^\/weapon\/weapons_parameter$/,
        action: function (args, request, response) {
          if (request.parameter_name !== 'skill_level') {
            return;
          }

//          GBF.updateWeaponSkillLevel(response);
        },
      },
      {
        path: /^\/weapon\/weapon\/\d+$/,
        action: function (args, request, response) {
          if (GBF.weaponLock) {
            GBF.weaponLock();
            GBF.weaponLock = null;
          }
//          GBF.updateWeaponSkillLevel(response);
        },
      },
    ],
    metasets: {},
    weapons: {},
    paramToWeapon: {},
    weaponLock: null,
  };


  function parseURL (url) {
    var a = document.createElement('a');
    a.href = url;
    return {
      host: a.hostname,
      path: a.pathname,
    };
  }


  function dispatch (path) {
    for (var ajaxAction of GBF.ajaxActionList) {
      var args = path.match(ajaxAction.path);
      if (!args) {
        continue;
      }
      return ajaxAction.action.bind(null, args);
    }

    return () => {};
  }


  function subco (g, rv) {
    var next = g.next(rv);
    var p = next.value;
    if (next.done) {
      return Promise.resolve(p);
    }
    return p.then((rv) => {
      return subco(g, rv);
    });
  }

  function co (gfn) {
    var g = gfn();
    return subco(g);
  };


  function * collectWeapon (nbWeapon) {
    for (var i = 0; i < nbWeapon; ++i) {
      var j = i % 20;
      var w = $($('#lis-weapon').children()[j]);
      w.trigger('tap');
      yield waitWeaponData();
      yield waitHuman();
      $('.prt-lead-link').trigger('tap');
      yield waitHuman();
      if (i % 20 == 19) {
        w = $('.btn-forward').trigger('tap');
        yield waitWeaponData();
        yield waitHuman();
      }
    }
  }


  function waitHuman () {
    var interval = Math.random() * 2000 + 1000;
    return new Promise((resolve) => {
      setTimeout(resolve, interval);
    });
  }


  function waitWeaponData () {
    return new Promise((resolve) => {
      if (GBF.weaponLock) {
        GBF.weaponLock();
      }
      GBF.weaponLock = resolve;
    });
  }


  GBF.addMetaset = function (number, sets) {
    GBF.metasets[number] = sets.map((set) => {
      return {
        name: set.name,
        job: set.job,
        weapon: set.weapon,
        summon: set.summon,
      };
    });
  };


  GBF.addWeapons = function (weapons) {
    for (var weapon of weapons) {
      var id = weapon.master.id;
      GBF.weapons[id] = weapon;
      GBF.paramToWeapon[weapon.param.id] = id;
    }
  };


  GBF.updateWeaponSkillLevel = function (weaponSkillMap) {
    for (var id in weaponSkillMap) {
      if (!GBF.paramToWeapon[id]) {
        continue;
      }
      var weaponID = GBF.paramToWeapon[id];
      GBF.weapons[weaponID].skillLevel = weaponSkillMap[id];
    }
  };


  GBF.run = function () {
    $(document).on('ajaxSuccess', function (event, xhr, jqXHR, response) {
      try {
        var url = parseURL(jqXHR.url);
        var action = dispatch(url.path);
        var request = jqXHR.data ? JSON.parse(jqXHR.data) : null;
        action(request, response);
      } catch (e) {
        console.error(e);
      }
    });
  };

  GBF.collectWeapon = function () {
    var tmp = $('.txt-possessed-weapon').text().match(/(\d+)\s*\//);
    var nbWeapon = parseInt(tmp[1], 10);
    return subco(collectWeapon(nbWeapon));
  };

  return GBF;

}


window.addEventListener('load', function () {
  var GBF = factory(unsafeWindow.$);
  GBF.run();
  unsafeWindow.__GBF__ = GBF;
});
