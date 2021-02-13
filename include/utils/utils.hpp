#pragma once

#include <sstream>
#include <cstdlib>
#include <regex>
#include <iostream>
#include <unistd.h>
#include <thread>

#define get_value(x) x.second

typedef std::map<std::string, std::pair<std::string, std::string>> key_to_pair_list;

int setAuthorizationFieldsFromEnvVariables(key_to_pair_list &authorization_fields) {
	for (const auto &[key, value] : authorization_fields) {
		std::string env_variable_name = value.first;
		std::string *env_variable_value = &authorization_fields[key].second;
		if (const char *env_value = std::getenv(env_variable_name.c_str())) {
			*env_variable_value = env_value;
		} else {
			std::cout << "Environment variable " << env_variable_name << " has no value set." << std::endl;
			return 1;
		}
	}
	return 0;
}