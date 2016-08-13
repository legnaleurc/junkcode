// ==UserScript==
// @name         GBF
// @namespace    foolproofproject
// @version      0.1
// @description  try to take over the world!
// @author       You
// @match        http://game.granbluefantasy.jp/*
// @grant        unsafeWindow
// @grant        GM_openInTab
// ==/UserScript==

'use strict';


function factory ($) {
  var GBF = {
    ajaxActionList: [
      // {
      //   path: /^\/deckcombination\/deck_combination_list\/(\d+)$/,
      //   action: function (args, request, response) {
      //     var metasetNumber = parseInt(args[1], 10);
      //     var sets = response.list;

      //     GBF.addMetaset(metasetNumber, sets);
      //   },
      // },
      {
        path: /^\/weapon\/list\/\d+\/0$/,
        action: function (args, request, response) {
          if (GBF.weaponLock) {
            GBF.weaponLock();
            GBF.weaponLock = null;
          }
        },
      },
      {
        path: /^\/weapon\/weapon\/\d+$/,
        action: function (args, request, response) {
          GBF.addWeapon(response);

          if (GBF.weaponLock) {
            GBF.weaponLock();
            GBF.weaponLock = null;
          }
        },
      },
    ],
    metasets: {},
    weapons: {},
    weaponLock: null,
  };

  class Weapon {

    constructor (weapon) {
      this._ = {
        id: weapon.id,
        master: weapon.master,
        param: weapon.param,
        skill1: weapon.skill1.level.took_skill ? new WeaponSkill(weapon.skill1) : null,
        skill2: weapon.skill2.level.took_skill ? new WeaponSkill(weapon.skill2) : null,
      };
    }

    static toJSON (key, value) {
      if (value instanceof Weapon) {
        return value._;
      } else if (value instanceof WeaponSkill) {
        return value._;
      }
      return value;
    }

    get id () {
      return this._.id;
    }

    get name () {
      return this._.master.name;
    }

    get attack () {
      return parseInt(this._.param.attack, 10);
    }

    get imageURL () {
      return `http://game-a.granbluefantasy.jp/assets/img_mid/sp/assets/weapon/b/${this._.master.id}.png`;
    }

    get skillLevel () {
      return parseInt(this._.param.skill_level, 10);
    }

    get skill1 () {
      return this._.skill1;
    }

    get skill2 () {
      return this._.skill2;
    }

  }


  class WeaponSkill {

    constructor (skill) {
      this._ = skill;
    }

    get name () {
      return this._.name;
    }

    get comment () {
      return this._.comment;
    }

  }


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
  }


  function * collectWeapon (nbWeapon) {
    for (var i = 0; i < nbWeapon; ++i) {
      var j = i % 20;
      var w = $('#lis-weapon').children();
      w = $(w[j]).trigger('tap');
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


  GBF.addWeapon = function (weapon) {
    var id = weapon.id;
    GBF.weapons[id] = new Weapon(weapon);
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
    return subco(collectWeapon(nbWeapon)).then(() => {
      var tmp = JSON.stringify(GBF.weapons, Weapon.toJSON);
      tmp = encodeURIComponent(tmp);
      tmp = unescape(tmp);
      tmp = btoa(tmp);
      GM_openInTab(`http://localhost:8000/#${tmp}`);
    });
  };

  return GBF;

}


window.addEventListener('load', function () {
  var GBF = factory(unsafeWindow.$);
  GBF.run();
  unsafeWindow.__GBF__ = GBF;
});
