import { eq } from "drizzle-orm";

import type { Filter, MutableFilter } from "./schema";
import { filters } from "./schema";
import { connect } from "./database";

export function selectFilterList(): Array<Filter> {
  return connect().select().from(filters).all();
}

export function insertFilter(filter: MutableFilter): Filter {
  return connect().insert(filters).values(filter).returning().get();
}

export function updateFilter(id: number, filter: MutableFilter): Filter {
  return connect()
    .update(filters)
    .set(filter)
    .where(eq(filters.id, id))
    .returning()
    .get();
}

export function deleteFilter(id: number): void {
  connect().delete(filters).where(eq(filters.id, id)).run();
}
