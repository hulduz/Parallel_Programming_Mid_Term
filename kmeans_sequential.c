#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include "struct.h"

void initialize_points(Point* points, int n) {
    for (int i = 0; i < n; i++) {
        points[i].x = rand() / (double)RAND_MAX;
        points[i].y = rand() / (double)RAND_MAX;
        points[i].cluster_id = -1; // Initialement, aucun cluster assigné
    }
}

void initialize_clusters(Cluster* clusters, Point* points, int k) {
    for (int i = 0; i < k; i++) {
        clusters[i].centroid_x = points[rand() % k].x;
        clusters[i].centroid_y = points[rand() % k].y;
        clusters[i].size = 0;
        clusters[i].new_centroid_x = 0.0;
        clusters[i].new_centroid_y = 0.0;
    }
}

int closest_cluster(Point p, Cluster* clusters, int k) {
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

void assign_clusters(Point* points, Cluster* clusters, int n, int k) {
    for (int i = 0; i < k; i++) {
        clusters[i].size = 0;
        clusters[i].new_centroid_x = 0.0;
        clusters[i].new_centroid_y = 0.0;
    }

    for (int i = 0; i < n; i++) {
        int cluster_id = closest_cluster(points[i], clusters, k);
        points[i].cluster_id = cluster_id;
        
        clusters[cluster_id].new_centroid_x += points[i].x;
        clusters[cluster_id].new_centroid_y += points[i].y;
        clusters[cluster_id].size++;
    }
}

void update_centroids(Cluster* clusters, int k) {
    for (int i = 0; i < k; i++) {
        if (clusters[i].size > 0) {
            clusters[i].centroid_x = clusters[i].new_centroid_x / clusters[i].size;
            clusters[i].centroid_y = clusters[i].new_centroid_y / clusters[i].size;
        }
    }
}

void kmeans(Point* points, Cluster* clusters, int n, int k, int iterations) {
    initialize_clusters(clusters, points, k);

    for (int iter = 0; iter < iterations; iter++) {
        assign_clusters(points, clusters, n, k);
        update_centroids(clusters, k);
    }
}
