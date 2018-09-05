class FileSystem {

  constructor (baseURL) {
    this._baseURL = baseURL;
  }

  async info (id_or_path) {
    return await this._get('/api/v1/info', [
      ['id_or_path', id_or_path],
    ]);
  }

  async list (id_or_path) {
    return await this._get('/api/v1/list', [
      ['id_or_path', id_or_path],
    ]);
  }

  async _get (path, params) {
    return await this._ajax('GET', path, params);
  }

  async _ajax (method, path, params) {
    const url = new URL(`${this._baseURL}${path}`);
    let body = null;
    if (method === 'GET') {
      for (const [k, v] of params) {
        url.searchParams.append(k, v);
      }
    } else {
      body = new URLSearchParams();
      for (const [k, v] of params) {
        body.append(k, v);
      }
    }
    const rqst = new Request(url, {
      method,
      cache: 'no-cache',
      body,
      mode: 'cors',
    });
    let rv = await fetch(rqst);
    rv = await rv.json();
    return rv;
  }

}


export {
  FileSystem,
};
