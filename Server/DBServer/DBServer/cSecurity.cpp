#include "cSecurity.h"

string Security::GenerateSalt()
{
	unsigned char salt[SALT_LENGTH];
	if (RAND_bytes(salt, SALT_LENGTH) == false) {
		DEBUGMSGNOPARAM("Salt Generate Failed\n");
	}

	string hexSalt;

	for (int i = 0; i < SALT_LENGTH; ++i) {
		char hex[3];
		sprintf_s(hex, "%02x", salt[i]);
		hexSalt += hex;
	}

	return hexSalt;
}

string Security::HashingPassword(const string password, const string salt)
{
	string str = password + salt;
	unsigned char hash[SHA256_DIGEST_LENGTH];

	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);

	string hashedStr;

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		char hex[3];
		sprintf_s(hex, "%02x", hash[i]);
		hashedStr += hex;
	}

	return hashedStr;
}

bool Security::VerifyPassword(string password, string hashedPassword, string salt)
{
	if (hashedPassword != HashingPassword(password, salt)) {
		return false;
	}

	return true;
}
