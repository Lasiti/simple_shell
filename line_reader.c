#include "main.h"

const short MAX_BUF_SIZE = 1024;
static char BUF[1024];
char DEFAULT_PS1[] = "$ ";
static int i;

/**
 * handle_signal - Handles a signal received by the program
 * @sig_num: The signal's code
 */
void handle_signal(int sig_num)
{
	signal(SIGINT, handle_signal);
	if (sig_num == SIGINT)
	{
		*((int *)get_shell_prop(NODE_EXIT_CODE_ID)) = EC_CONTROL_C_TERMINATION;
		write(STDOUT_FILENO, "\n", 1);
		print_prompt();
	}
}

/**
 * get_cmd_line - Reads a line of characters from the stdin
 *
 * Return: A pointer to the string
 */
char *get_cmd_line()
{
	int n = 0, j, k, len = 0;
	char *line = NULL, stop = FALSE, error = FALSE, quote = 0, quote_o = FALSE;

	i = 0;
	while (!stop)
	{
		n = read(STDIN_FILENO, (void *)BUF, MAX_BUF_SIZE);
		if (n < 0)
		{
			if (line != NULL)
				free(line);
			return (NULL);
		}
		for (j = 0; (BUF[i + j] != '\n') && (j < n) && (i + j < MAX_BUF_SIZE); j++)
			;
		line = _realloc(line, sizeof(char) * len, sizeof(char) * (len + j));
		if (line != NULL)
		{
			for (k = 0; (BUF[i] != '\n') && (k < j) && (i < MAX_BUF_SIZE); k++)
			{
				check_chars(&quote, &quote_o, BUF[i]);
				*(line + len) = BUF[i], len++, i++;
			}
			stop = (BUF[i] == '\n' ? TRUE : stop);
			i = (i >= MAX_BUF_SIZE ? 0 : i);
		}
		stop = (n == 0 ? TRUE : stop);
	}
	set_error(&error, quote_o, n, line, j), handle_ctrl_d(len);
	line = _realloc(line, sizeof(char) * len, sizeof(char) * (len + 1));
	if (line != NULL)
	{
		if (error == TRUE)
			free(line), line = NULL;
		else
			*(line + len) = '\0';
	}
	return (line);
}

/**
 * check_chars - Checks the characters for quote termination
 * @quote: The pointer to the current open quote
 * @quote_o: The pointer the open status of the current quote
 * @current_char: The current character
 */
void check_chars(char *quote, char *quote_o, char current_char)
{
	if (is_quote(current_char))
	{
		if (*quote == 0)
		{
			*quote = current_char;
			*quote_o = TRUE;
		}
		else if (current_char == *quote)
		{
			*quote = 0;
			*quote_o = FALSE;
		}
	}
}

/**
 * set_error - Checks for some errors in the command line
 * @error: The pointer to the error flag
 * @quote_o: The status of the last quote in the command line
 * @n: The last number of characters read
 * @str: The command line
 * @pos: The length of the command line string
 */
void set_error(char *error, char quote_o, int n, char *str, int pos)
{
	char *buf0, *buf1;
	char ops_at_end = FALSE;

	if (pos > 0)
		ops_at_end = (str[pos - 1] == '|') || (str[pos - 1] == '&') ? TRUE : FALSE;

	if (((n == 0) && ((quote_o == TRUE) || ops_at_end))
		|| ((quote_o == TRUE) || ops_at_end))
	{
		buf0 = *((char **)get_shell_prop(EXEC_NAME_ID));
		buf1 = long_to_str(get_line_num());

		write(STDERR_FILENO, buf0, str_len(buf0));
		write(STDERR_FILENO, ": ", 2);
		write(STDERR_FILENO, buf1, str_len(buf1));
		if (quote_o == TRUE)
			write(STDERR_FILENO, ": Syntax error: Unterminated quoted string\n", 43);
		else
			write(STDERR_FILENO, ": Syntax error: end of file unexpected\n", 39);
		*error = TRUE;
		fflush(stderr);
		free(buf1);
	}
}

/**
 * print_prompt - Prints the prompt for this shell program
 */
void print_prompt()
{
	char *ps1;
	int j, n;
	char **vars = NULL, *var_val = NULL;

	if (*((char *)get_shell_prop(IS_INTERACTIVE_ID)) == TRUE)
	{
		ps1 = get_env_var("PS1");
		if (ps1 == NULL)
			ps1 = str_copy(DEFAULT_PS1);
		else
			ps1 = str_copy(ps1);
		vars = get_variables(ps1, &n);
		if (vars != NULL)
		{
			for (j = 0; j < n; j++)
			{
				if (str_eql("$?", *(vars + j)))
					var_val = long_to_str(*((int *)get_shell_prop(NODE_EXIT_CODE_ID)));
				else if (str_eql("$$", *(vars + j)))
					var_val = long_to_str(*((int *)get_shell_prop(SHELL_PID_ID)));
				else
					var_val = str_copy(get_env_var(*(vars + j) + 1));
				ps1 = str_replace(ps1, *(vars + j), var_val, TRUE);
			}
			free(vars);
		}
		write(STDOUT_FILENO, ps1, str_len(ps1));
		fflush(stdout);
		free(ps1);
	}
}
