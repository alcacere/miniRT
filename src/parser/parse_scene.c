#include "minirt.h"

void	parse_line(char *line, t_scene *scene)
{
	char	**tokens;
	size_t	line_len;

	line_len = ft_strlen(line);
	if (line && line_len > 0 && line[line_len - 1] == '\n')
		line[line_len - 1] = '\0';
	tokens = ft_split_charset(line, " \t");
	if (!tokens || !tokens[0] || tokens[0][0] == '#')
	{
		free_split(tokens);
		return ;
	}
	if (ft_strncmp(tokens[0], "A", 2) == 0)
		parse_ambient(tokens, scene);
	else if (ft_strncmp(tokens[0], "C", 2) == 0)
		parse_camera(tokens, scene);
	else if (ft_strncmp(tokens[0], "L", 2) == 0)
		parse_light(tokens, scene);
	else if (ft_strncmp(tokens[0], "sp", 3) == 0 || \
			ft_strncmp(tokens[0], "pl", 3) == 0 || \
			ft_strncmp(tokens[0], "cy", 3) == 0)
		parse_shape(tokens, scene);
	else
		print_error_exit("TODO");
	free_split(tokens);
}
