#include "Validator.h"

bool Validator::isPasswordValid(string password)
{
	if (password.length() < 4) return false;
    if (password.find(" ") != string::npos) return false;
    if (password.find_first_of("0123456789") == string::npos) return false;
    if (password.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == string::npos) return false;
    if (password.find_first_of("abcdefghijklmnopqrstuvwxyz") == string::npos) return false;
    return true;
}

bool Validator::isUsernameValid(string username)
{
	if ((username[0] <= 122 && username[0] >= 97) || (username[0] <= 90 && username[0] >= 65)) return false;
    if (username.find(" ") != string::npos) return false;
    if (username.length() == 0) return false;
    return true;
}