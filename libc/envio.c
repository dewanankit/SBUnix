#include "envio.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

//`char *env[];

int  getenv(char* argv, char *envp[])
{

	int j = 0;
	while(envp[j] != 0) {
		char **tokens = (char**)malloc(sizeof(char*)*10);
		strtok(envp[j], '=', tokens);
		if((strcmp(tokens[0], argv) == 0)) {
			printf("%s  ", tokens[1]);
		}
		j++;
	}
	return 0;
}
//        int l=0;
//        //char *env_local[1000];
//
//
//        l=0;
////	int ret_value = -1;
//  //      char *tokens[1000];
//        int ret_val = -1;
//        int found = 0;
//        while(global_env[l] != 0 && found == 0)
//        {
//                char * t = global_env[l];
//                int pos = 0;
//              //  int match = 0;
//                while(t[pos] != '=' && env_var[pos]!= '\0')
//                {
//                        if(t[pos]!=env_var[pos])
//                        {
//				break;
//                        }
//			pos++;
//                }
//		if(t[pos] == '=')
//		{
//			ret_val = l;
//			found = 1;
//		}
//		l++;
//        }
//        return ret_val;
//}

//void setenv(char *env_var, char *env_value)
//{
//	int env_var_nbr;
//	env_var_nbr = getenv(env_var);
//	char *str = env_var;
//	str = strcat(str,"=");
//	str = strcat(str,env_value);
//	global_env[env_var_nbr] = str;
//}
