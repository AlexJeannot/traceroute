#include "../inc/traceroute.h"

/*
 * Allocate space for error message
 * Create error message
 * Pass it to error_exit function
*/
void	error_msg_exit(char *option, char *arg)
{
	if (!(env.error_msg = (char *)malloc(sizeof(char) * (ft_strlen(option) + ft_strlen(arg) + 13))))
		error_exit("Error message memory allocation failed");
	sprintf(env.error_msg, "invalid %s: '%s'", option, arg);
	error_exit(env.error_msg);
}

/*
* Prepare error message depending on getaddrinfo return
*/
void	addrinfo_error(char *target, int error_code)
{
	char	*error;

	error = NULL;
	if (error_code == EAI_NONAME)
		error = "name or service is unknown";
	else if (error_code == EAI_AGAIN)
		error = "Temporary failure in name resolution";
	else if (error_code == EAI_FAIL)
		error = "Non-recoverable failure in name res";
	else if (error_code == EAI_MEMORY)
		error = "Memory allocation failure";
	else if (error_code == EAI_SYSTEM)
		error = "System error";
	else
		error = "Error in adress resolution";

	error_msg_exit(target, error);
}
