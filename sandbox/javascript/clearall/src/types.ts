export type CallbackParams = {
  code: string;
  state: string;
};

export type Token = {
  refresh_token?: string;
  access_token?: string;
  token_type?: string;
  expires_in?: number;
  scope?: string;
  expires_at?: Date;
};

export interface TwitterResponseError extends Error {
  status: number;
  statusText: string;
  headers: Record<string, string>;
  error: {
    title: string;
    detail: string;
    type: string;
    status: number;
  };
}
