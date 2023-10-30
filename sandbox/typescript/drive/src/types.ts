export type FileMeta = {
  id: string;
  name: string;
  isTrashed: boolean;
  ctime: Date;
  mtime: Date;
  parentId: string;
  isFolder: boolean;
  mimeType: string;
  hash: string;
  size: number;
  extra: Record<string, any>;
};

export interface FileSystem {
  mkdir(name: string, parentId: string): Promise<FileMeta>;
  move(fileId: string, newName: string, newParentId: string): Promise<void>;
  trash(fileId: string): Promise<void>;
  download(fileId: string): Promise<ReadableFile>;
  upload(fileId: string): Promise<WritableFile>;
}

export interface RandomAccessFile {
  open(): Promise<void>;
  close(): Promise<void>;
  seek(offset: number): Promise<void>;
  tell(): Promise<number>;
}

export interface ReadableFile extends RandomAccessFile {
  read(numBytes: number): Promise<Uint8Array | null>;
}

export interface WritableFile extends RandomAccessFile {
  write(data: Uint8Array): Promise<number>;
  flush(): Promise<void>;
}
