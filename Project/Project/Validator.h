#pragma once

#include <vector>
#include <map>
#include <string>
#include "User.h"
#include "DataBase.h"

using namespace std;

class Validator{
  public:
    static bool isPasswordValid(string password);
    static bool isUsernameValid(string username);
};