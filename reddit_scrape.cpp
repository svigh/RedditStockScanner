#include <sstream>
#include <cstdlib>
#include <regex>

#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include "curlpp/Options.hpp"
#include "curlpp/Exception.hpp"
#include "curlpp/Infos.hpp"
#define get_value(x) x.second

typedef std::map<std::string, std::pair<std::string, std::string>> key_to_pair_list;

class RequestManager {
	private:
		static inline std::string header_response;	// has to be inline to be seen as a variable and to not declare it out of class

		static size_t HeaderCallback(char* buffer, size_t size, size_t nmemb)
		{
			int totalSize = size * nmemb;
			header_response += std::string(buffer, totalSize);
			return totalSize;
		}
	public:
		RequestManager() {
			std::cout << "Created request" << std::endl;
		}
		int send_post_request(std::string url, std::list<std::string> request_headers, std::string request_body, std::string user_agent, std::string user_password, std::string &response_header, std::string &response_body) {
			try {
				curlpp::Easy request;
				std::stringstream writeStream;

#ifdef VERBOSE
				request.setOpt(new curlpp::options::Verbose(true));
#endif
				request.setOpt(new curlpp::options::Url(url));
				request.setOpt(new curlpp::options::UserAgent(user_agent));
				request.setOpt(new curlpp::options::WriteStream(&writeStream));	// To get the body
				request.setOpt(new curlpp::options::HttpHeader(request_headers));
				request.setOpt(new curlpp::options::HeaderFunction(HeaderCallback)); // To get the header
				request.setOpt(new curlpp::options::PostFields(request_body));
				request.setOpt(new curlpp::options::UserPwd(user_password));

				request.perform();

				std::string body(std::istreambuf_iterator<char>(writeStream), {});
				response_header = header_response;
				response_body = body;

				return EXIT_SUCCESS;
			}
			catch ( curlpp::LogicError & e ) {
				std::cout << e.what() << std::endl;
			}
			catch ( curlpp::RuntimeError & e ) {
				std::cout << e.what() << std::endl;
			}

			return EXIT_FAILURE;
		}
};

class RedditAPI {
	private:
		RequestManager requestManager;
		std::string access_token_url = "https://www.reddit.com/api/v1/access_token";
		std::string m_refresh_token = "";
		std::string current_access_token = "";
		std::string user_agent = "123";
		std::string response_header, response_body;
		key_to_pair_list m_authorization_fields;

		void setCurrentAccessToken() {
			int pos = 0;
			int token_length_offset = 31;
			if ((pos = response_body.find("access_token")) != std::string::npos) {
				int start_of_token = pos + sizeof("access_token\":\"");
				current_access_token = std::string(&response_body[start_of_token], &response_body[start_of_token + token_length_offset]);
			}
		}
		void setRefreshToken() {
			int pos = 0;
			int token_length_offset = 31;
			if ((pos = response_body.find("refresh_token")) != std::string::npos) {
				int start_of_token = pos + sizeof("refresh_token\":\"");
				current_access_token = std::string(&response_body[start_of_token], &response_body[start_of_token + token_length_offset]);
			}
		}
	public:
		RedditAPI(key_to_pair_list authorization_fields) {
			m_authorization_fields = authorization_fields;
			std::cout << "Generating new refresh_token and access_token" << std::endl;
			generateRefreshToken(user_agent, "permanent");
		}
		RedditAPI(key_to_pair_list authorization_fields, std::string refresh_token){
			m_authorization_fields = authorization_fields;
			std::cout << "Received refresh_token, generating new access_token" << std::endl;
			m_refresh_token = refresh_token;
			updateAccessToken(user_agent);
		}

		// // {"access_token": "-BUvJUh3q4zoUXUZK8u62YHlOz-j88A", "token_type": "bearer", "expires_in": 3600, "refresh_token": "-q9Fd0L6QSFLiYmpo4fgqseNYO8-2rw", "scope": "*"}
		int generateRefreshToken(std::string user_agent, std::string duration) {
			std::list<std::string> headers = {""};
			std::string body = "grant_type=client_credentials&duration=" + duration; //&username=" + username + "&password=" + password;
			int ret_code = requestManager.send_post_request(
				access_token_url,
				headers,
				body,
				user_agent,
				get_value(m_authorization_fields["bot_id"]) + ":" + get_value(m_authorization_fields["bot_secret"]),
				response_header, response_body
			);
			setRefreshToken();
			setCurrentAccessToken();
			return ret_code;
		}

		// {"access_token": "-V5G4q2WBNog9KqXWsSbLjUdIVe26GA", "token_type": "bearer", "expires_in": 3600, "scope": "*"}
		int updateAccessToken(std::string user_agent) {
			std::list<std::string> headers = {""};
			std::string body = "grant_type=refresh_token&refresh_token=" + m_refresh_token; //"&username=" + username + "&password=" + password;

			int ret_code = requestManager.send_post_request(
				access_token_url,
				headers,
				body,
				user_agent,
				get_value(m_authorization_fields["bot_id"]) + ":" + get_value(m_authorization_fields["bot_secret"]),
				response_header, response_body
			);
			setCurrentAccessToken();
			return ret_code;
		}

		std::string getResponseHeader() {
			return response_header;
		}

		std::string getResponseBody() {
			return response_body;
		}

		key_to_pair_list getAuthorizationFields() {
			return m_authorization_fields;
		}
};

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

int main(int arg_count, char **arguments)
{
	// KEY : [LINUX_ENV_VAR_NAME : VALUE]
	key_to_pair_list authorization_fields = {
		// {"username" , {"REDDIT_USERNAME", ""} },
		// {"password" , {"REDDIT_PASSWORD", ""} },
		{"bot_id" , {"BOT_ID", ""} },
		{"bot_secret" , {"BOT_SECRET", ""} }
	};
	setAuthorizationFieldsFromEnvVariables(authorization_fields);

	std::cout << get_value(authorization_fields["username"]) << std::endl;
	std::cout << get_value(authorization_fields["password"]) << std::endl;
	std::cout << get_value(authorization_fields["bot_id"]) << std::endl;
	std::cout << get_value(authorization_fields["bot_secret"]) << std::endl;

	RedditAPI reddit = RedditAPI(authorization_fields, "-mkN07zkkB4NmziSE17RoF3wgsztk-g");
	std::cout << reddit.getResponseBody() << std::endl;
	// std::cout << reddit.getResponseHeader() << std::endl;
	// std::cout << "RESPONSE HEADER IS\n" << response_header << std::endl;
	// std::cout << "RESPONSE BODY IS\n" << response_body << std::endl;
	return 0;
}











