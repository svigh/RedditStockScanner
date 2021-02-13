#pragma once

#include "request_manager.hpp"
#include "../utils/utils.hpp"

class RedditAPI {
	private:
		RequestManager requestManager;
		std::string access_token_url = "https://www.reddit.com/api/v1/access_token";
		std::string me_info_url = "https://oauth.reddit.com/api/v1/me";
		std::string refresh_token = "";
		std::string current_access_token = "";
		std::string user_agent = "123";
		std::string response_header, response_body;
		key_to_pair_list authorization_fields;

		// TODO: get tokens another way, get a json parser
		int setCurrentAccessToken() {
			int pos = 0;
			int token_length_offset = 31;
			if ((pos = response_body.find("access_token")) != std::string::npos) {
				int start_of_token = pos + sizeof("access_token\":\"");
				current_access_token = std::string(&response_body[start_of_token], &response_body[start_of_token + token_length_offset]);
			} else {
				std::cerr << "Could not retrieve access token from body" << std::endl;
				std::cerr << response_body << std::endl;
				return 1;
			}
			return 0;
		}

		// TODO: get tokens another way, get a json parser
		int setRefreshToken() {
			int pos = 0;
			int token_length_offset = 31;
			if ((pos = response_body.find("refresh_token")) != std::string::npos) {
				int start_of_token = pos + sizeof("refresh_token\":\"");
				refresh_token = std::string(&response_body[start_of_token], &response_body[start_of_token + token_length_offset]);
			} else {
				std::cerr << "Could not retrieve refresh token from body" << std::endl;
				std::cerr << response_body << std::endl;
				return 1;
			}
			return 0;
		}

		RedditAPI(const RedditAPI &){}
	public:

		RedditAPI(key_to_pair_list _authorization_fields) {
			authorization_fields = _authorization_fields;
			std::cout << "Generating new refresh_token and access_token" << std::endl;
			generateRefreshToken();
		}
		RedditAPI(key_to_pair_list _authorization_fields, std::string _refresh_token){
			authorization_fields = _authorization_fields;
			std::cout << "Received refresh_token, generating new access_token" << std::endl;
			refresh_token = _refresh_token;
			updateAccessToken();
		}
		RedditAPI(key_to_pair_list _authorization_fields, std::string _refresh_token, std::string _current_access_token){
			authorization_fields = _authorization_fields;
			refresh_token = _refresh_token;
			current_access_token = _current_access_token;
			std::cout << "Received refresh_token and access_token" << std::endl;
		}

		// // {"access_token": "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "token_type": "bearer", "expires_in": 3600, "refresh_token": "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "scope": "*"}
		int generateRefreshToken() {
			std::list<std::string> headers = {""};
			std::string body = "grant_type=client_credentials&duration=permanent"; // + duration + "&username=" + username + "&password=" + password;

			int ret_code = requestManager.send_post_request(
				access_token_url,
				headers,
				body,
				user_agent,
				get_value(authorization_fields["bot_id"]) + ":" + get_value(authorization_fields["bot_secret"]),
				response_header, response_body
			);
			setRefreshToken();
			setCurrentAccessToken();
			return ret_code;
		}

		// {"access_token": "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "token_type": "bearer", "expires_in": 3600, "scope": "*"}
		int updateAccessToken() {
			std::list<std::string> headers = {""};
			std::string body = "grant_type=refresh_token&refresh_token=" + refresh_token; //"&username=" + username + "&password=" + password;

			int ret_code = requestManager.send_post_request(
				access_token_url,
				headers,
				body,
				user_agent,
				get_value(authorization_fields["bot_id"]) + ":" + get_value(authorization_fields["bot_secret"]),
				response_header, response_body
			);
			setCurrentAccessToken();
			return ret_code;
		}

		int getMeInfo() {
			std::list<std::string> headers = {"Authorization: Bearer " + current_access_token};

			int ret_code = requestManager.send_get_request(
				me_info_url,
				headers,
				user_agent,
				response_header, response_body
			);

			return ret_code;
		}

		std::string getResponseHeader() {
			return response_header;
		}

		std::string getResponseBody() {
			return response_body;
		}

		key_to_pair_list getAuthorizationFields() {
			return authorization_fields;
		}

		std::string getRefreshToken() {
			return refresh_token;
		}

		std::string getCurrentAccessToken() {
			return current_access_token;
		}
};