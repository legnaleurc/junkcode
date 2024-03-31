import { json } from "@sveltejs/kit";

import { insertFilter, selectFilterList } from "$lib/server/filters";

import type { RequestHandler } from "./$types";

export const GET = (async () => {
  const filterList = selectFilterList();
  return json(filterList);
}) satisfies RequestHandler;

export const POST = (async ({ request }) => {
  const data = await request.json();
  const filter = insertFilter(data);
  return json(filter);
}) satisfies RequestHandler;
