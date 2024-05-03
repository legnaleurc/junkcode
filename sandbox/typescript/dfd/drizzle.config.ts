import "dotenv/config";
import type { Config } from "drizzle-kit";

export default {
  schema: "./src/lib/server/schema.ts",
  out: "./drizzle/migrations",
  driver: "better-sqlite",
  dbCredentials: {
    url: process.env.SQLITE_FILE,
  },
} satisfies Config;
