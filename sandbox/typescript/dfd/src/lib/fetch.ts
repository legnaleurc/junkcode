type FetchFunction = typeof fetch;

export class RequestBuilder<T> {
  private _fetch: FetchFunction;
  private _method: string;
  private _base: string;
  private _path: string;
  private _body: string;

  constructor() {
    this._fetch = fetch;
    this._method = "GET";
    this._base = "";
    this._path = "";
    this._body = "";
  }

  use(fetch: FetchFunction): RequestBuilder<T> {
    this._fetch = fetch;
    return this;
  }

  get(): RequestBuilder<T> {
    this._method = "GET";
    return this;
  }

  post(): RequestBuilder<T> {
    this._method = "POST";
    return this;
  }

  patch(): RequestBuilder<T> {
    this._method = "PATCH";
    return this;
  }

  delete(): RequestBuilder<T> {
    this._method = "DELETE";
    return this;
  }

  base(base: string): RequestBuilder<T> {
    this._base = base;
    return this;
  }

  path(path: string): RequestBuilder<T> {
    this._path = path;
    return this;
  }

  jsonBody(payload: unknown): RequestBuilder<T> {
    this._body = JSON.stringify(payload);
    return this;
  }

  async response(): Promise<Response> {
    if (!this._path || !this._base) {
      throw new Error("invalid url");
    }
    const headers: HeadersInit = {
      Accepts: "application/json",
      "Content-Type": "application/json",
    };
    const options: RequestInit = {
      method: this._method,
      headers,
    };
    if (this._body) {
      options.body = this._body;
    }
    const url = new URL(this._path, this._base);
    const request = new Request(url, options);
    const response = await this._fetch(request);
    if (!response.ok) {
      throw new Error(response.statusText);
    }
    return response;
  }

  async fetch(): Promise<T> {
    const response = await this.response();
    return await response.json();
  }
}
