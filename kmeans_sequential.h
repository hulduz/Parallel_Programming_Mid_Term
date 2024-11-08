#ifndef KMEANS_SEQ_H
#define KMEANS_SEQ_H
#include "struct.h"

void initialize_points(Point* points, int n);
void initialize_clusters(Cluster* clusters, Point* points, int k);
void assign_clusters(Point* points, Cluster* clusters, int n, int k);
void update_centroids(Point* points, Cluster* clusters, int n, int k);
void kmeans(Point* points, Cluster* clusters, int n, int k, int iterations);

#endif /* KMEANS_SEQ_H */