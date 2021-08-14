#include "main.h"

/**
 * get_env_var - Retrieves the value of an environment variable
 * @var: The name of the environment variable
 *
 * Return: The pointer to the environment variable's value, otherwise, NULL
 */
char *get_env_var(char *var)
{
	char **envp = *((char ***)get_shell_prop(ENVP_ID));
	int n = *((int *)get_shell_prop(ENVP_COUNT_ID));
	int i, j;

	if (envp != NULL)
	{
		for (i = 0; i < n; i++)
		{
			for (j = 0; (*(var + j) != '\0') && (*(*(envp + i) + j) != '\0'); j++)
			{
				if ((*(*(envp + i) + j) != *(var+ j)) || (*(*(envp + i) + j) == '='))
					break;
			}
			if (*(*(envp + i) + j) == '=' && *(var + j) == '\0')
				return (*(envp + i) + j  + 1);
		}
	}
	return (NULL);
}

/**
 * set_env_var - Sets the value of an environment variable
 * @var: The name of the environment variable
 * @val: The value of the environment variable
 */
void set_env_var(char *var, char*val)
{
	char **envp = *((char ***)get_shell_prop(ENVP_ID));
	int n = *((int *)get_shell_prop(ENVP_COUNT_ID));
	int i, j;

	if (envp != NULL)
	{
		for (i = 0; i < n; i++)
		{
			for (j = 0; (*(var + j) != '\0') && (*(*(envp + i) + j) != '\0'); j++)
			{
				if ((*(*(envp + i) + j) != *(var+ j)) || (*(*(envp + i) + j) == '='))
					break;
			}
			if (*(*(envp + i) + j) == '=' && *(var + j) == '\0')
			{
				if (val != NULL)
				{
					*(envp + i) = str_cat(str_cat(var, "=", FALSE), str_copy(val), TRUE);
					break;
				}
			}
		}
	}
}

/**
 * add_env_var - Adds an environment variable
 * @var: The name of the environment variable
 * @val: The value of the environment variable
 */
void add_env_var(char *var, char*val)
{
	char **envp = *((char ***)get_shell_prop(ENVP_ID));
	int *n = (int *)get_shell_prop(ENVP_COUNT_ID);
	int i, j;

	if (envp != NULL)
	{
		if (get_env_var(var) != NULL)
		{
			set_env_var(var, val);
		}
		else
		{
			*(envp + *n) = str_cat(str_cat(var, "=", FALSE), str_copy(val), TRUE);
			(*n)++;
		}
	}
}

/**
 * remove_env_var - Removes an environment variable
 * @var: The name of the environment variable
 */
void remove_env_var(char *var)
{
	char **envp_n = NULL;
	char **envp = *((char ***)get_shell_prop(ENVP_ID));
	int *n = (int *)get_shell_prop(ENVP_COUNT_ID);
	int i, j, k = 0;

	if (envp != NULL)
	{
		if (get_env_var(var) != NULL)
		{
			for (i == 0; i < *n; i++)
			{
				for (j = 0; (*(var + j) != '\0') && (*(*(envp + i) + j) != '\0'); j++)
				{
					if ((*(*(envp + i) + j) != *(var+ j)) || (*(*(envp + i) + j) == '='))
						break;
				}
				if (!(*(*(envp + i) + j) == '=' && *(var + j) == '\0'))
				{
					envp_n = _realloc(envp_n, sizeof(void *) * k, sizeof(void *) * (k + 1));
					*(envp_n + k) = str_copy(*(envp + i));
					free(*(envp + i));
					k++;
				}
				else
				{
					free(*(envp + i));
					(*n)--;
				}
			}
			free(envp);
			envp = envp_n;
		}
	}
}
