import type { Client } from "twitter-api-sdk";

export async function getMyId(client: Client): Promise<string> {
  const myself = await client.users.findMyUser();
  const userId = myself.data?.id!;
  return userId;
}

export async function* iterTweetList(client: Client, userId: string) {
  let pagination_token: string | undefined;
  while (true) {
    const tweetList = await client.tweets.usersIdTweets(userId, {
      max_results: 100,
      "tweet.fields": ["created_at"],
      pagination_token,
    });
    if (!tweetList.data) {
      return;
    }
    for (const tweet of tweetList.data) {
      yield tweet;
    }
    pagination_token = tweetList.meta?.next_token;
    if (!pagination_token) {
      break;
    }
  }
}
