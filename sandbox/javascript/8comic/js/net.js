function ajax (method, responseType, url) {
  return new Promise(function (resolve, reject) {
    var xhr = new XMLHttpRequest({
      mozAnon: true,
      mozSystem: true,
    });
    xhr.open(method, url);
    xhr.responseType = responseType;
    xhr.addEventListener('load', function (event) {
      var self = event.target;
      resolve({
        code: self.status,
        body: self.response,
      });
    });
    xhr.addEventListener('error', function (event) {
      reject(event);
    });
    xhr.send();
  });
}


export function getPage (url) {
  return ajax('GET', 'document', url).then(function (response) {
    return response.body;
  });
}
