define('view', ['core', 'parser'], function (parser) {
  'use strict';
  
  function View () {
  }
  View.extend = core.extend;
  
  
  function LatestUpdateView () {
    this._summaries = [];
  }
  
  LatestUpdateView.prototype.show = function () {
    return parser.getLatest().then(function (comicList) {
      this._summaries = comicList.map(function (comic) {
        return new SummaryView(comic);
      });
    }.bind(this)).then(function () {
      var content = document.querySelector('#content');
      this._summaries.forEach(function (summaryView) {
        summaryView.show(content);
      });
    }.bind(this)).then(undefined, function (e) {
      console.info('latest update view show', e);
    });
  }
  
  
  function SummaryView (comic) {
    this._title = comic.title;
    this._url = comic.url;
    this._el = document.createElement('div');
    this._el.classList.add('comic-entry');
    var title = document.createElement('div');
    title.classList.add('title');
    title.textContent = this._title;
    this._el.appendChild(title);
  }
  
  SummaryView.prototype.show = function (parent) {
    parent.appendChild(this._el);
    
    parser.getSummary(this._url).then(function (summary) {
      var url = summary.coverURL;
      this._el.style.backgroundImage = `url("${url}")`;
    }.bind(this)).then(undefined, function (e) {
      console.info('summary view show', e);
    });
  };
  
  return {
    LatestUpdateView: LatestUpdateView,
  };
});
