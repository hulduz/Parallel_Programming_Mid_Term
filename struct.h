#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    double x;
    double y;
    int cluster_id;
} Point;

typedef struct {
    int size;
    double centroid_x;
    double centroid_y;
    // Used to calculate the new centroid
    double new_centroid_x;
    double new_centroid_y;
} Cluster;

#endif