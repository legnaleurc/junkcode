import { PUBLIC_API_BASE } from "$env/static/public";

import { RequestBuilder } from "$lib/fetch";

type ReadOnly = {
  id: number;
};
type ReadWrite = {
  regexp: string;
};

export type Filter = ReadOnly & ReadWrite;
export type MutableFilter = ReadWrite;

export function getFilterList(): RequestBuilder<Array<Filter>> {
  return new RequestBuilder<Array<Filter>>()
    .base(PUBLIC_API_BASE)
    .path("/api/v1/filters");
}

export function postFilter(filter: MutableFilter): RequestBuilder<Filter> {
  return new RequestBuilder<Filter>()
    .base(PUBLIC_API_BASE)
    .path("/api/v1/filters")
    .post()
    .jsonBody(filter);
}

export function patchFilter(
  id: number,
  filter: MutableFilter,
): RequestBuilder<Filter> {
  return new RequestBuilder<Filter>()
    .base(PUBLIC_API_BASE)
    .path(`/api/v1/filters/${id}`)
    .patch()
    .jsonBody(filter);
}

export function deleteFilter(id: number): RequestBuilder<number> {
  return new RequestBuilder<number>()
    .base(PUBLIC_API_BASE)
    .path(`/api/v1/filters/${id}`)
    .delete();
}
