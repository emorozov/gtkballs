/* path.c - functions related to finding path between points
 * on board
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* returns x coordinate of the node
   number_of_cells: number of cells in the row */
int __find_x_of_the_node(int node, int number_of_x_cells, int number_of_y_cells) {
    assert(number_of_x_cells > 0);
    return node % number_of_x_cells;
}

/* returns y coordinate of the node
   number_of_cells: number of cells in the row
*/
int __find_y_of_the_node(int node, int number_of_x_cells, int number_of_y_cells) {
    assert(number_of_x_cells > 0);
    return node / number_of_x_cells;
}

/* returns x and y coordinates of the node */
void find_x_y_of_the_node(int *x, int *y, int node, int number_of_x_cells, int number_of_y_cells) {
    *x = __find_x_of_the_node(node, number_of_x_cells, number_of_y_cells);
    *y = __find_y_of_the_node(node, number_of_x_cells, number_of_y_cells);
}

/* returns node at x and y coordinates */
int find_node_of_x_y(int x, int y, int number_of_x_cells) {
    return y * number_of_x_cells + x;
}

/* find number of the node by its coordinates
   number_of_cells: number of cells in the row */
int find_node_by_coords(int x, int y, int number_of_x_cells, int number_of_y_cells) {
    return y * number_of_x_cells + x;
}

/* mark all neighbouring nodes of the nodes
   source_nodes: source nodes
   neighbours: neighbouring nodes
   mark: number to mark neighbours
   number_of_cells: number of cells in the row
   returns number of marked nodes */
int mark_neighbours_of_the_nodes(int *nodes, int *source_nodes, int *neighbours, int mark,
                                 int number_of_x_cells, int number_of_y_cells) {
    int i, j, neighbours_count = 0;
    int x, y, node;
    int xses[4] = {0, 0, 1, -1};
    int yses[4] = {-1, 1, 0, 0};

    for (i = 1; i <= source_nodes[0]; i++) {
        find_x_y_of_the_node(&x, &y, source_nodes[i], number_of_x_cells, number_of_y_cells);
        if (x != -1 && y != -1) {
            for (j = 0; j < 4; j++) {
                if (x + xses[j] >= 0 && x + xses[j] < number_of_x_cells &&
                    y + yses[j] >= 0 && y + yses[j] < number_of_y_cells) {
                    node = find_node_by_coords(x + xses[j], y + yses[j], number_of_x_cells, number_of_y_cells);
                    if ((node != -1) && nodes[node] == 0) {
                        nodes[node] = mark;
                        neighbours[++neighbours_count] = node;
                    }
                }
            }
        }
    }
    neighbours[0] = neighbours_count;
    return neighbours_count;
}

/* check if nodes are neighbours */
int is_neighbours(int node1, int node2, int xn, int yn) {
    int x1, y1, x2, y2;

    find_x_y_of_the_node(&x1, &y1, node1, xn, yn);
    find_x_y_of_the_node(&x2, &y2, node2, xn, yn);

    if ((x1 == x2) && ((y1 == y2 + 1) || (y2 == y1 + 1))) {
        return 1;
    } else if ((y1 == y2) && ((x1 == x2 + 1) || (x2 == x1 + 1))) {
        return 1;
    }
    return 0;
}

/* find the path between source_node and the target_node
   result stored in path
   returns 0 on failure and 1 on success */
int find_path(int *nodes, int source_node, int target_node, int *path,
              int number_of_x_cells, int number_of_y_cells) {
    int waves[number_of_x_cells * number_of_y_cells][number_of_x_cells * number_of_y_cells];
    int i, j, k = 1, finish = 0;

    waves[0][0] = 1;
    waves[0][1] = source_node;

    nodes[source_node] = -1;
    while (!finish) {
        if (!mark_neighbours_of_the_nodes(nodes, waves[k - 1], waves[k], k,
                                          number_of_x_cells, number_of_y_cells)) {
            /* the destination can never be reached */
            return 0;
        }
        for (i = 1; i <= waves[k][0]; i++) {
            if (waves[k][i] == target_node) {
                finish = 1;
                break;
            }
        }
        k++;
    }

    path[0] = k;
    path[1] = waves[k - 1][i];
    for (j = k - 2; j > 0; j--) {
        finish = 0;
        for (i = 1; i <= waves[j][0]; i++) {
            if (is_neighbours(waves[j][i], path[k - j - 1],
                              number_of_x_cells, number_of_y_cells)) {
                path[k - j] = waves[j][i];
                break;
            }
        }
    }

    return 1;
}
