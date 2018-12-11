export class FileSystem {

  constructor (baseURL) {
    this._baseURL = baseURL;
  }

  async root () {
    const node = await this._get('/api/v1/nodes/root');
    return node;
  }

  async list (id) {
    let children = await this._get(`/api/v1/nodes/${id}/children`);
    children = children.map(node => {
      node.created = Date.parse(node.created);
      node.modified = Date.parse(node.modified);
      return node;
    });
    return children;
  }

  async searchByName (name) {
    const pathList = await this._get('/api/v1/nodes', {
      name,
    });
    return pathList;
  }

  async move (srcList, id) {
    for (const chunk of chunksOf(srcList, 10)) {
      const requestList = chunk.map(src => this._patch(`/api/v1/nodes/${src}`, {
        parent_id: id,
      }));
      await Promise.all(requestList);
    }
  }

  async trash (srcList) {
    for (const chunk of chunksOf(srcList, 10)) {
      const requestList = chunk.map(src => this._delete(`/api/v1/nodes/${src}`));
      await Promise.all(requestList);
    }
  }

  async imageList (id) {
    return await this._get(`/api/v1/nodes/${id}/images`);
  }

  stream (id) {
    return `${this._baseURL}/api/v1/nodes/${id}/stream`;
  }

  image (id, imageId) {
    return `${this._baseURL}/api/v1/nodes/${id}/images/${imageId}`;
  }

  async sync () {
    return await this._post('/api/v1/changes');
  }

  async _get (path, params) {
    return await this._ajax('GET', path, params);
  }

  async _post (path, params) {
    return await this._ajax('POST', path, params);
  }

  async _patch (path, params) {
    return await this._ajax('PATCH', path, params);
  }

  async _delete (path, params) {
    return await this._ajax('DELETE', path, params);
  }

  async _ajax (method, path, params) {
    const url = new URL(`${this._baseURL}${path}`);
    let body = null;
    if (params) {
      if (method === 'GET') {
        Object.keys(params).forEach(k => {
          url.searchParams.append(k, params[k]);
        });
      } else {
        body = JSON.stringify(params);
      }
    }
    const rqst = new Request(url, {
      method,
      cache: 'no-cache',
      body,
      mode: 'cors',
    });
    let rv = await fetch(rqst);
    if (rv.status !== 204) {
      rv = await rv.json();
    }
    return rv;
  }

}


export function classNameFromObject(o) {
  const keys = Object.keys(o);
  const classList = keys.filter(key => o[key]);
  return classList.join(' ');
}


function * chunksOf (array, size) {
  for (let i = 0; i < array.length; i += size) {
    yield array.slice(i, i + size);
  }
}
