#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "struct.h"

int closest_cluster_parallel(Point p, Cluster* clusters, int k) {
    int closest = 0;
    double min_dist = pow(p.x - clusters[0].centroid_x, 2) + pow(p.y - clusters[0].centroid_y, 2);

    for (int i = 1; i < k; i++) {
        double dist = pow(p.x - clusters[i].centroid_x, 2) + pow(p.y - clusters[i].centroid_y, 2);
        if (dist < min_dist) {
            min_dist = dist;
            closest = i;
        }
    }
    return closest;
}

void initialize_clusters_parallel(Cluster* clusters, Point* points, int n, int k) {
    for (int i = 0; i < k; i++) {
        clusters[i].centroid_x = points[rand() % n].x;
        clusters[i].centroid_y = points[rand() % n].y;
        clusters[i].size = 0;
        clusters[i].new_centroid_x = 0.0;
        clusters[i].new_centroid_y = 0.0;
    }
}

void assign_clusters_parallel(Point* points, Cluster* clusters, int n, int k) {
    for (int i = 0; i < k; i++) {
        clusters[i].size = 0;
        clusters[i].new_centroid_x = 0.0;
        clusters[i].new_centroid_y = 0.0;
    }

    #pragma omp parallel
    {
        int* local_sizes = (int*)calloc(k, sizeof(int));
        double* local_sums_x = (double*)calloc(k, sizeof(double));
        double* local_sums_y = (double*)calloc(k, sizeof(double));

        #pragma omp for
        for (int i = 0; i < n; i++) {
            int cluster_id = closest_cluster_parallel(points[i], clusters, k);
            points[i].cluster_id = cluster_id;

            local_sizes[cluster_id]++;
            local_sums_x[cluster_id] += points[i].x;
            local_sums_y[cluster_id] += points[i].y;
        }

        #pragma omp critical
        {
            for (int i = 0; i < k; i++) {
                clusters[i].size += local_sizes[i];
                clusters[i].new_centroid_x += local_sums_x[i];
                clusters[i].new_centroid_y += local_sums_y[i];
            }
        }

        free(local_sizes);
        free(local_sums_x);
        free(local_sums_y);
    }
}

void update_centroids_parallel(Cluster* clusters, int k) {
    for (int i = 0; i < k; i++) {
        if (clusters[i].size > 0) {
            clusters[i].centroid_x = clusters[i].new_centroid_x / clusters[i].size;
            clusters[i].centroid_y = clusters[i].new_centroid_y / clusters[i].size;
        }
    }
}

void kmeans_parallel(Point* points, Cluster* clusters, int n, int k, int iterations) {
    initialize_clusters_parallel(clusters, points, n, k);

    for (int iter = 0; iter < iterations; iter++) {
        assign_clusters_parallel(points, clusters, n, k);
        update_centroids_parallel(clusters, k);
    }
}
