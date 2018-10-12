export class FileSystem {

  constructor (baseURL) {
    this._baseURL = baseURL;
  }

  async root () {
    return await this._get('/api/v1/root');
  }

  async info (id) {
    return await this._get(`/api/v1/info/${id}`);
  }

  async list (id) {
    let children = await this._get(`/api/v1/list/${id}`);
    children = children.map(node => {
      node.created = Date.parse(node.created);
      node.modified = Date.parse(node.modified);
      return node;
    });
    children.sort((l, r) => {
      if (l.modified < r.modified) {
        return 1;
      }
      if (l.modified > r.modified) {
        return -1;
      }
      return 0;
    });
    return children;
  }

  file (id) {
    return `${this._baseURL}/api/v1/file/${id}`;
  }

  async sync () {
    return await this._post('/api/v1/sync');
  }

  change () {
    const base = this._baseURL.replace('http', 'ws');
    const ws = new WebSocket(`${base}/socket/v1/changes`);
    return ws;
  }

  async _get (path, params) {
    return await this._ajax('GET', path, params);
  }

  async _post (path, params) {
    return await this._ajax('POST', path, params);
  }

  async _ajax (method, path, params) {
    const url = new URL(`${this._baseURL}${path}`);
    let body = null;
    if (params) {
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


export function classNameFromObject(o) {
  const keys = Object.keys(o);
  const classList = keys.filter(key => o[key]);
  return classList.join(' ');
}
