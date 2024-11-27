#ifndef KMEANS_PARALLEL_H
#define KMEANS_PARALLEL_H
#include "struct.h"

void initialize_points_parallel(Point* points, int n);
void initialize_clusters_parallel(Cluster* clusters, Point* points, int n, int k); // Ajout "_par"
void assign_clusters_parallel(Point* points, Cluster* clusters, int n, int k);
void update_centroids_parallel(Cluster* clusters, int k); // Ajout "_par"
void kmeans_parallel(Point* points, Cluster* clusters, int n, int k, int iterations); // Ajout "_par"

#endif /* KMEANS_PARALLEL_H */
