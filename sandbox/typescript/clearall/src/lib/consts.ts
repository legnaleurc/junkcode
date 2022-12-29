import * as dotenv from "dotenv";

dotenv.config();

export const CLIENT_ID = process.env.CLIENT_ID!;
export const CLIENT_SECRET = process.env.CLIENT_SECRET!;
export const CALLBACK = process.env.CALLBACK!;
export const TOKEN_FILE = ".token.json";
export const INTERVAL_MS = 1000 * 60 * 15;
export const TOKEN_MS = 1000 * 60 * 30;
