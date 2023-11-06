import { Drive, Service } from "./types";

type CreateDriveParameters = {
    createService: () => Promise<Service>;
    createSnapshot: () => Promise<Service>;
};
export async function createDrive(params: CreateDriveParameters): Promise<Drive> {
    return new DefaultDrive();
}

class DefaultDrive implements Drive {
    async sync() {}

    async * walk() {}
}
