import { Drive, FileService, SnapshotService } from "./types";

type CreateDriveParameters = {
    createFileService: () => Promise<FileService>;
    createSnapshotService: () => Promise<SnapshotService>;
};
export async function createDrive(params: CreateDriveParameters): Promise<Drive> {
    return new DefaultDrive();
}

class DefaultDrive implements Drive {
    async sync() {}

    async * walk() {}
}
