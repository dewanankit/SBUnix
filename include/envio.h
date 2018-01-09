#ifndef __ENVIO_H
#define __ENVIO_H

//char **global_env;

void setenv(char *env_var, char *env_value);
int  getenv(char* argv, char *envp[]);

#endif
