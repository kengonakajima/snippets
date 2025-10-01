export type ClientMessage = {
  type: 'user-message';
  body: string;
};

export type ServerMessage =
  | {
      type: 'status';
      body: string;
    }
  | {
      type: 'openai-event';
      body: unknown;
    };
