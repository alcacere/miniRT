#include "parse.h"
#include "libft.h"
#include <fcntl.h>
#include <stdlib.h>

static int	route_element(char **tokens, t_scene *scene)
{
	if (!tokens || !tokens[0])
		return (1);
	if (ft_strncmp(tokens[0], "A", 2) == 0)
		return (parse_ambient(tokens, scene));
	else if (ft_strncmp(tokens[0], "C", 2) == 0)
		return (parse_camera(tokens, scene));
	else if (ft_strncmp(tokens[0], "L", 2) == 0)
		return (parse_light(tokens, scene));
	else if (ft_strncmp(tokens[0], "sp", 3) == 0)
		return (parse_sphere(tokens, scene));
	else if (ft_strncmp(tokens[0], "pl", 3) == 0)
		return (parse_plane(tokens, scene));
	else if (ft_strncmp(tokens[0], "cy", 3) == 0)
		return (parse_cylinder(tokens, scene));
	else if (ft_strcmp(tokens[0], "tr") == 0)
		return (parse_triangle(tokens, scene));
	else if (tokens[0][0] == '#')
		return (1);
	return (0);
}

int	parse_line(char *line, t_scene *scene)
{
	char	**tokens;
	int		status;

	tokens = ft_split_charset(line, " \t\v\r\f\n");
	if (!tokens)
		return (0);
	if (!tokens[0])
	{
		ft_free_arr(tokens);
		return (1);
	}
	
	status = route_element(tokens, scene);
	ft_free_arr(tokens);
	return (status);
}

int	parse_file(const char *filename, t_scene *scene)
{
	int		fd;
	char	*line;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (0);
	line = get_next_line(fd);
	while (line)
	{
		if (!parse_line(line, scene))
		{
			free(line);
			close(fd);
			return (0);
		}
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (1);
}