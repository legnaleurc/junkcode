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
    const children = await this._get(`/api/v1/list/${id}`);
    children.sort((l, r) => {
      if (l.name < r.name) {
        return -1;
      }
      if (l.name > r.name) {
        return 1;
      }
      return 0;
    });
    return children;
  }

  file (id) {
    return `${this._baseURL}/api/v1/file/${id}`;
  }

  async _get (path, params) {
    return await this._ajax('GET', path, params);
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


export function createNode(node) {
  return {
    id: node.id,
    name: node.name,
    children: node.is_folder ? [] : null,
  };
}


export function classNameFromObject(o) {
  const keys = Object.keys(o);
  const classList = [];
  for (const key of keys) {
    if (o[key]) {
      classList.push(key);
    }
  }
  return classList.join(' ');
}
