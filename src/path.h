#ifndef __GTKBALLS_PATH_H
#define __GTKBALLS_PATH_H

int find_path (int *nodes, int source_node, int target_node, int *path,
                int number_of_x_cells, int number_of_y_cells);

void find_x_y_of_the_node (int *x, int *y,
                            int node, int number_of_x_cells, int number_of_y_cells);

int find_node_of_x_y (int x, int y, int number_of_x_cells);

#endif
