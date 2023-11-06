import { Drive, FileService, SnapshotService } from "./types";

type CreateDriveParameters = {
    createService: () => Promise<FileService>;
    createSnapshot: () => Promise<SnapshotService>;
};
export async function createDrive(params: CreateDriveParameters): Promise<Drive> {
    return new DefaultDrive();
}

class DefaultDrive implements Drive {
    async sync() {}

    async * walk() {}
}
