define('view', ['parser'], function (parser) {
  'use strict';
  
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
    }.bind(this));
  }
  
  function SummaryView (comic) {
    this._title = comic.title;
    this._url = comic.url;
    this._el = document.createElement('div');
    this._el.classList.add('comic-entry');
    var title = document.createElement('p');
    title.textContent = this._title;
    this._el.appendChild(title);
  }
  
  SummaryView.prototype.show = function (parent) {
    parent.appendChild(this._el);
    
    parser.getSummary(this._url).then(function (summary) {
      var img = document.createElement('img');
      img.src = summary.coverURL;
      this._el.appendChild(img);
    }.bind(this));
  };
  
  return {
    LatestUpdateView: LatestUpdateView,
  };
});
