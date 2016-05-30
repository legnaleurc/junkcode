(function (context, $) {
  'use strict';

  var GBF = {
    ajaxActionList: [
      {
        path: /^\/deckcombination\/deck_combination_list\/(\d+)$/,
        action: function (args, data) {
          var metasetNumber = parseInt(args[1], 10);
          var sets = data.list;

          GBF.addMetaset(metasetNumber, sets);
        },
      },
      {
        path: /^\/weapon\/list\/\d+\/0$/,
        action: function (args, data) {
          var weapons = data.list;

          GBF.addWeapons(weapons);
        },
      },
    ],
    metasets: {},
    weapons: {},
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
    }
  };


  GBF.load = function () {
    $(document).on('ajaxSuccess', function (event, xhr, jqXHR, data) {
      var url = parseURL(jqXHR.url);
      var action = dispatch(url.path);
      action(data);
    });
  };


  GBF.unload = function () {
    $(document).off('ajaxSuccess');
  };


  if (context.__gbf__) {
    context.__gbf__.unload();
  }
  context.__gbf__ = GBF;
  GBF.load();

})(window, jQuery);
