#pragma once

#include "stdafx.h"
#include <openssl/sha.h>
#include <openssl/rand.h>

class Security {
public:
	string GenerateSalt();

	string HashingPassword(const string password, const string salt);

	bool VerifyPassword(string password, string hashedPassword, string salt);

private:
};