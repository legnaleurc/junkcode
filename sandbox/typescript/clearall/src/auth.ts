import { readFile, writeFile } from "fs/promises";

import { auth } from "twitter-api-sdk";

import type { CallbackParams, Token } from "./types";
import { CLIENT_ID, CLIENT_SECRET, CALLBACK, TOKEN_FILE } from "./consts";
import { readAll, writeString } from "./io";

export async function createAuthUser(): Promise<auth.OAuth2User> {
  let token = await readToken();
  const authUser = new auth.OAuth2User({
    client_id: CLIENT_ID,
    client_secret: CLIENT_SECRET,
    callback: CALLBACK,
    scopes: ["users.read", "tweet.read", "tweet.write", "offline.access"],
    token,
  });

  if (!token) {
    token = await createToken(authUser);
    hackExpireDate(token);
    await writeToken(token);
  }

  if (authUser.isAccessTokenExpired()) {
    const data = await authUser.refreshAccessToken();
    await writeToken(data.token);
  }

  return authUser;
}

function generateAuthUrl(authUser: auth.OAuth2User): string {
  const url = authUser.generateAuthURL({
    code_challenge_method: "s256",
    state: "access_token",
  });
  return url;
}

function parseCallbackUrl(rawUrl: string): CallbackParams {
  const url = new URL(rawUrl);
  return {
    code: url.searchParams.get("code") ?? "",
    state: url.searchParams.get("state") ?? "",
  };
}

async function createToken(authUser: auth.OAuth2User): Promise<Token> {
  const authUrl = generateAuthUrl(authUser);
  writeString(process.stdout, authUrl + "\n");
  const rawInput = await readAll(process.stdin);
  const params = parseCallbackUrl(rawInput.trim());
  const { token } = await authUser.requestAccessToken(params.code);
  return token;
}

async function readToken(): Promise<Token | undefined> {
  try {
    const buffer = await readFile(TOKEN_FILE, {
      encoding: "utf-8",
    });
    const token = JSON.parse(buffer) as Token;
    return token;
  } catch (e: unknown) {
    return undefined;
  }
}

async function writeToken(token: Token): Promise<void> {
  await writeFile(TOKEN_FILE, JSON.stringify(token), {
    encoding: "utf-8",
  });
}

function hackExpireDate(token: Token): void {
  if (!token.expires_at) {
    return;
  }
  token.expires_at -= 60 * 1000;
}
