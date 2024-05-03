import { drizzle } from "drizzle-orm/better-sqlite3";
import Database from "better-sqlite3";

import { SQLITE_FILE } from "$env/static/private";

export function connect() {
  const sqlite = new Database(SQLITE_FILE);
  const database = drizzle(sqlite);
  return database;
}
