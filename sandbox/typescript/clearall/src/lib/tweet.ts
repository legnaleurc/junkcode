import type { Client } from "twitter-api-sdk";

export async function getMyId(client: Client): Promise<string> {
  const myself = await client.users.findMyUser();
  const userId = myself.data?.id!;
  return userId;
}

export async function blockUserId(
  client: Client,
  userId: string,
  targetUserId: string,
) {
  const rv = await client.users.usersIdBlock(userId, {
    target_user_id: targetUserId,
  });
  return rv.data?.blocking ?? false;
}

export async function* iterUserFollowing(client: Client, userId: string) {
  let token: string | undefined;
  while (true) {
    const rv = await client.users.usersIdFollowing(userId, {
      max_results: 1000,
      pagination_token: token,
    });
    if (!rv.data) {
      return;
    }
    for (const user of rv.data) {
      yield user;
    }
    token = rv.meta?.next_token;
    if (!token) {
      break;
    }
  }
}

export async function* iterTweetList(client: Client, userId: string) {
  let token: string | undefined;
  while (true) {
    const rv = await client.tweets.usersIdTweets(userId, {
      max_results: 100,
      "tweet.fields": ["created_at"],
      pagination_token: token,
    });
    if (!rv.data) {
      return;
    }
    for (const tweet of rv.data) {
      yield tweet;
    }
    token = rv.meta?.next_token;
    if (!token) {
      break;
    }
  }
}

export async function* iterListsIncludesUserId(client: Client, userId: string) {
  let token: string | undefined;
  while (true) {
    const rv = await client.lists.getUserListMemberships(userId, {
      max_results: 100,
      "list.fields": ["private", "description", "owner_id"],
      pagination_token: token,
    });
    if (!rv.data) {
      return;
    }
    for (const list of rv.data) {
      yield list;
    }
    token = rv.meta?.next_token;
    if (!token) {
      break;
    }
  }
}
