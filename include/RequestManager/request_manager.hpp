#pragma once

#include "curlpp/cURLpp.hpp"
#include "curlpp/Easy.hpp"
#include "curlpp/Options.hpp"
#include "curlpp/Exception.hpp"
#include "curlpp/Infos.hpp"

class RequestManager {
	private:
		static inline std::string header_response;	// has to be inline to be seen as a variable and to not declare it out of class
		curlpp::Easy request;
		static size_t HeaderCallback(char* buffer, size_t size, size_t nmemb)
		{
			int totalSize = size * nmemb;
			header_response += std::string(buffer, totalSize);
			return totalSize;
		}
	public:
		RequestManager(){}
		~RequestManager(){}

		int send_post_request(std::string url, std::list<std::string> request_headers, std::string request_body, std::string user_agent, std::string user_password, std::string &response_header, std::string &response_body) {
			try {
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
				request.reset();

				// Extract the body from the write stream
				std::string body(std::istreambuf_iterator<char>(writeStream), {});

				response_header = header_response;
				response_body = body;
#ifdef VERBOSE
				std::cout << response_header << std::endl;
				std::cout << response_body << std::endl;
#endif
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

		int send_get_request(std::string url, std::list<std::string> request_headers, std::string user_agent, std::string &response_header, std::string &response_body) {
			try {
				std::stringstream writeStream;

#ifdef VERBOSE
				request.setOpt(new curlpp::options::Verbose(true));
#endif
				request.setOpt(new curlpp::options::Url(url));
				request.setOpt(new curlpp::options::UserAgent(user_agent));
				request.setOpt(new curlpp::options::WriteStream(&writeStream));	// To get the body
				request.setOpt(new curlpp::options::HttpHeader(request_headers));
				request.setOpt(new curlpp::options::HeaderFunction(HeaderCallback)); // To get the header
				request.perform();
				request.reset();

				// Extract the body from the write stream
				std::string body(std::istreambuf_iterator<char>(writeStream), {});

				response_header = header_response;
				response_body = body;
#ifdef VERBOSE
				std::cout << response_header << std::endl;
				std::cout << response_body << std::endl;
#endif
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