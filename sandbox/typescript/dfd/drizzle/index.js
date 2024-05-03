import { migrate } from "drizzle-orm/better-sqlite3/migrator";
import { drizzle } from "drizzle-orm/better-sqlite3";
import Database from "better-sqlite3";

const { SQLITE_FILE } = process.env;

const sqlite = new Database(SQLITE_FILE);
try {
  const database = drizzle(sqlite);
  migrate(database, {
    migrationsFolder: "drizzle/migrations",
  });
} finally {
  sqlite.close();
}
