CREATE PROCEDURE Authenticate (
    IN InUsername VARCHAR(255),
    IN InPassword VARCHAR(255),
    IN InIterations INT,
    IN InAddressIP VARCHAR(45),
    IN InAuthKey VARCHAR(32),
    IN InEmailVerificationEnabled BOOLEAN,
    OUT OutLoginStatus INT,
    OUT OutAccountStatus INT,
    OUT OutAccountID INT
)
BEGIN
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
    DECLARE TempPasswordHash VARBINARY(80);
    DECLARE IsPasswordValid BOOLEAN;

    SET OutLoginStatus = LOGIN_STATUS_ERROR;
    SET OutAccountStatus = ACCOUNT_STATUS_OUT_OF_SERVICE;
    SET OutAccountID = NULL;

    SELECT AccountID, PasswordHash, EmailVerified, DeletedAt
    INTO OutAccountID, TempPasswordHash, TempEmailVerified, TempDeletedAt
    FROM Accounts
    WHERE Username = InUsername;

    IF OutAccountID IS NULL THEN
        SET OutAccountStatus = ACCOUNT_STATUS_INVALID_CREDENTIALS;
    ELSE
        IF TempDeletedAt IS NOT NULL THEN
            SET OutAccountStatus = ACCOUNT_STATUS_ACCOUNT_IS_DELETED;
        ELSEIF InEmailVerificationEnabled AND NOT TempEmailVerified THEN
            SET OutAccountStatus = ACCOUNT_STATUS_ACCOUNT_NOT_VERIFIED;
        ELSE
            CALL VerifyPassword(InPassword, TempPasswordHash, InIterations, IsPasswordValid);

            IF IsPasswordValid THEN
                SET OutLoginStatus = LOGIN_STATUS_SUCCESS;
                SET OutAccountStatus = ACCOUNT_STATUS_NORMAL;

                IF EXISTS (SELECT 1 FROM Sessions WHERE AccountID = OutAccountID AND Online = TRUE) THEN
                    SET OutAccountStatus = ACCOUNT_STATUS_ALREADY_LOGGED_IN;
                ELSEIF EXISTS (SELECT 1 FROM Blacklists WHERE AccountID = OutAccountID) THEN
                    SET OutAccountStatus = ACCOUNT_STATUS_ACCOUNT_BANNED;
                ELSEIF EXISTS (SELECT 1 FROM Blacklists WHERE AddressIP = InAddressIP) THEN
                    SET OutAccountStatus = ACCOUNT_STATUS_IP_BANNED;
                ELSE
                    INSERT INTO Sessions (AccountID, AuthKey, AddressIP, Online, CreatedAt)
                    VALUES (OutAccountID, InAuthKey, InAddressIP, FALSE, UNIX_TIMESTAMP())
                    ON DUPLICATE KEY UPDATE AuthKey = InAuthKey, AddressIP = InAddressIP, UpdatedAt = UNIX_TIMESTAMP();
                END IF;
            ELSE
                SET OutAccountStatus = ACCOUNT_STATUS_INVALID_CREDENTIALS;
            END IF;
        END IF;
    END IF;
END;