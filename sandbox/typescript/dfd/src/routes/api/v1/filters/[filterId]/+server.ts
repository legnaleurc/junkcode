import { json } from "@sveltejs/kit";

import { deleteFilter, updateFilter } from "$lib/server/filters";

import type { RequestHandler } from "./$types";

export const PATCH = (async ({ params, request }) => {
  const { filterId } = params;
  const data = await request.json();
  const filter = updateFilter(Number(filterId), data);
  return json(filter);
}) satisfies RequestHandler;

export const DELETE = (async ({ params }) => {
  const { filterId } = params;
  deleteFilter(Number(filterId));
  return new Response(null, {
    status: 204,
  });
}) satisfies RequestHandler;
