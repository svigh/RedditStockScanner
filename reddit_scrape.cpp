#include "include/utils/utils.hpp"
#include "include/RedditAPI/reddit_api.hpp"

int main(int arg_count, char **arguments)
{
	// KEY : [LINUX_ENV_VAR_NAME : VALUE]
	key_to_pair_list authorization_fields = {
		{"bot_id" , {"BOT_ID", ""} },
		{"bot_secret" , {"BOT_SECRET", ""} }
	};
	setAuthorizationFieldsFromEnvVariables(authorization_fields);

	RedditAPI reddit = RedditAPI(authorization_fields, "-5AC9LSEseyMYhkvAsCVm-kVocYPkxg", "-8dRVs5YwDbKj78rntVOdMgcZwPBiDQ");
	// RedditAPI reddit = RedditAPI(authorization_fields);
	std::cout << reddit.getResponseBody() << std::endl;
	std::cout << reddit.getRefreshToken() << std::endl;
	std::cout << reddit.getCurrentAccessToken() << std::endl;

	reddit.getMeInfo();
	std::cout << reddit.getResponseBody() << std::endl;
	std::cout << reddit.getRefreshToken() << std::endl;
	std::cout << reddit.getCurrentAccessToken() << std::endl;
	return 0;
}
