#include "Server.h"
#include "ClientProtocol.h"

Void StartDisconnectTimer(
    ServerRef Server,
    SocketRef Socket,
    ClientContextRef Client,
    SocketConnectionRef Connection,
    UInt32 Timeout,
    UInt32 SystemMessage
) {
    S2C_DATA_DISCONNECT_TIMER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DISCONNECT_TIMER);
    Response->Timeout = Timeout;
    Client->Flags |= CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
    Client->DisconnectTimestamp = GetTimestampMs() + Timeout;
    SocketSend(Socket, Connection, Response);
}

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Context,
    UInt32 AuthKey,
    UInt16 EntityID
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if ((Client->Flags & CLIENT_FLAGS_CONNECTED) &&
            Client->AuthKey == AuthKey &&
            Connection->ID == EntityID) {
            return Client;
        }
    }

    return NULL;
}

Void ServerLoadMigrationData(
    ServerConfig Config,
    ServerContextRef Context
) {
    CString FilePath = PathCombineNoAlloc(Config.Database.MigrationDataPath, "MigrationRegistry.txt");
    if (!CLFileExists(FilePath)) {
        Fatal("Error loading migration registry: %s", FilePath);
    }

    FileRef File = fopen(FilePath, "r");
    if (!File) return;

    Char MigrationDirective[32] = { 0 };
    Char MigrationFileName[256] = { 0 };

    while (fscanf(File, "%31s %255s", MigrationDirective, MigrationFileName) == 2) {
        if (strcmp(MigrationDirective, "AddMigration") == 0) {
            CString MigrationFilePath = PathCombineNoAlloc(Config.Database.MigrationDataPath, MigrationFileName);
            if (!CLFileExists(MigrationFilePath)) Fatal("Migration file not found: %s", MigrationFilePath);

            FileRef MigrationFile = FileOpen(MigrationFilePath);
            if (!MigrationFile) Fatal("Error loading migration file: %s", MigrationFilePath);

            CString MigrationQuery = NULL;
            Int32 MigrationQueryLength = 0;
            if (!FileRead(MigrationFile, (UInt8**)&MigrationQuery, &MigrationQueryLength)) Fatal("Error loading migration file: %s", MigrationFilePath);

            Info("Running migration: %s", MigrationFileName);

            if (!DatabaseCallProcedure(
                Context->Database,
                "ApplyMigration",
                DB_INPUT_STRING(MigrationFileName, strlen(MigrationFileName)),
                DB_INPUT_STRING(MigrationQuery, strlen(MigrationQuery)),
                DB_PARAM_END
            )) {
                Fatal("Database migration failed");
            }

            free(MigrationQuery);
            FileClose(MigrationFile);
        }
        else {
            Error("Invalid directive: %s\n", MigrationDirective);
        }
    }

    fclose(File);
}

Void ServerClearState(
    ServerContextRef Context
) {
    DatabaseCallProcedure(Context->Database, "ClearSessionOnline", DB_PARAM_END);
}
