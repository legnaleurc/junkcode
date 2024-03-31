import { integer, sqliteTable, text } from "drizzle-orm/sqlite-core";

export const filters = sqliteTable("filters", {
  id: integer("id").primaryKey(),
  regexp: text("regexp").notNull(),
});

export type Filter = typeof filters.$inferSelect;
export type MutableFilter = typeof filters.$inferInsert;
