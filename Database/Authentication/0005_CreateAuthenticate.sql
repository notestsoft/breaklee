CREATE PROCEDURE Authenticate (
    IN InUsername VARCHAR(255),
    IN InAddressIP VARCHAR(45),
    IN InAuthKey VARCHAR(32),
    IN InEmailVerificationEnabled BOOLEAN,
    OUT OutLoginStatus INT,
    OUT OutAccountStatus INT,
    OUT OutAccountID INT,
    OUT OutPasswordHash VARBINARY(128)
)
BEGIN
    -- Define constants
    DECLARE ACCOUNT_STATUS_NORMAL INT DEFAULT 32;
    DECLARE ACCOUNT_STATUS_INVALID_CREDENTIALS INT DEFAULT 33;
    DECLARE ACCOUNT_STATUS_ALREADY_LOGGED_IN INT DEFAULT 34;
    DECLARE ACCOUNT_STATUS_OUT_OF_SERVICE INT DEFAULT 35;
    DECLARE ACCOUNT_STATUS_ACCOUNT_EXPIRED INT DEFAULT 36;
    DECLARE ACCOUNT_STATUS_IP_BANNED INT DEFAULT 37;
    DECLARE ACCOUNT_STATUS_ACCOUNT_BANNED INT DEFAULT 38;
    DECLARE ACCOUNT_STATUS_TEST_SERVER_TRIAL INT DEFAULT 39;
    DECLARE ACCOUNT_STATUS_PC_CAFE INT DEFAULT 40;
    DECLARE ACCOUNT_STATUS_ACCOUNT_NOT_VERIFIED INT DEFAULT 41;
    DECLARE ACCOUNT_STATUS_ACCOUNT_IS_DELETED INT DEFAULT 42;
    DECLARE ACCOUNT_STATUS_ACCOUNT_IS_LOCKED INT DEFAULT 43;

    DECLARE LOGIN_STATUS_ERROR INT DEFAULT 0;
    DECLARE LOGIN_STATUS_SUCCESS INT DEFAULT 1;
    
    DECLARE TempEmailVerified BOOLEAN;
    DECLARE TempDeletedAt BIGINT UNSIGNED;

    -- Default values
    SET OutLoginStatus = LOGIN_STATUS_ERROR;
    SET OutAccountStatus = ACCOUNT_STATUS_OUT_OF_SERVICE;
    SET OutAccountID = NULL;
    SET OutPasswordHash = NULL;

    -- Check if the username exists
    SELECT AccountID, PasswordHash, EmailVerified, DeletedAt
    INTO OutAccountID, OutPasswordHash, TempEmailVerified, TempDeletedAt
    FROM Accounts
    WHERE Username = InUsername;

    -- Account does not exist
    IF OutAccountID IS NULL THEN
        SET OutAccountStatus = ACCOUNT_STATUS_INVALID_CREDENTIALS;
    ELSE
        -- Account exists; check its status
        IF TempDeletedAt IS NOT NULL THEN
            SET OutAccountStatus = ACCOUNT_STATUS_ACCOUNT_IS_DELETED;
        ELSEIF InEmailVerificationEnabled AND NOT TempEmailVerified THEN
            SET OutAccountStatus = ACCOUNT_STATUS_ACCOUNT_NOT_VERIFIED;
        ELSE
            -- Set valid login status
            SET OutLoginStatus = LOGIN_STATUS_SUCCESS;
            SET OutAccountStatus = ACCOUNT_STATUS_NORMAL;

            -- Check session and blacklist
            IF EXISTS (SELECT 1 FROM Sessions WHERE AccountID = OutAccountID AND Online = TRUE) THEN
                SET OutAccountStatus = ACCOUNT_STATUS_ALREADY_LOGGED_IN;
            ELSEIF EXISTS (SELECT 1 FROM Blacklists WHERE AccountID = OutAccountID) THEN
                SET OutAccountStatus = ACCOUNT_STATUS_ACCOUNT_BANNED;
            ELSEIF EXISTS (SELECT 1 FROM Blacklists WHERE AddressIP = InAddressIP) THEN
                SET OutAccountStatus = ACCOUNT_STATUS_IP_BANNED;
            ELSE
                -- Generate new session
                INSERT INTO Sessions (AccountID, AuthKey, AddressIP, Online, CreatedAt)
                VALUES (OutAccountID, InAuthKey, InAddressIP, FALSE, UNIX_TIMESTAMP())
                ON DUPLICATE KEY UPDATE AuthKey = InAuthKey, UpdatedAt = UNIX_TIMESTAMP();
            END IF;
        END IF;
    END IF;
END;