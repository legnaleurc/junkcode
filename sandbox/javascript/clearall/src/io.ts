export function readAll(stream: NodeJS.ReadStream): Promise<string> {
  return new Promise((resolve, reject) => {
    const onReadable = () => {
      const chunk: Buffer = stream.read();
      stream.off("end", onEnd);
      if (chunk) {
        resolve(chunk.toString("utf-8"));
      } else {
        resolve("");
      }
    };
    const onEnd = () => {
      reject();
    };
    stream.once("readable", onReadable);
    stream.once("end", onEnd);
  });
}

export function writeString(stream: NodeJS.WriteStream, data: string): void {
  stream.write(data);
}

export async function wait(msDelay: number): Promise<void> {
  return new Promise((resolve) => {
    setTimeout(resolve, msDelay);
  });
}
