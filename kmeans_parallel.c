#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "struct.h"

// Fonction pour trouver le cluster le plus proche (inchangée)
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

void initialize_points(Point* points, int n) {
    for (int i = 0; i < n; i++) {
        points[i].x = rand() / (double)RAND_MAX;
        points[i].y = rand() / (double)RAND_MAX;
        points[i].cluster_id = -1; //Aucun cluster assigné initialement
    }
}

// Initialisation des clusters (inchangée)
void initialize_clusters(Cluster* clusters, Point* points, int n, int k) {
    for (int i = 0; i < k; i++) {
        clusters[i].centroid_x = points[rand() % n].x;
        clusters[i].centroid_y = points[rand() % n].y;
        clusters[i].size = 0;
        clusters[i].new_centroid_x = 0.0;
        clusters[i].new_centroid_y = 0.0;
    }
}

// Assignation des clusters en parallèle
void assign_clusters(Point* points, Cluster* clusters, int n, int k) {
    // Réinitialisation des clusters
    for (int i = 0; i < k; i++) {
        clusters[i].size = 0;
        clusters[i].new_centroid_x = 0.0;
        clusters[i].new_centroid_y = 0.0;
    }

    // Assignation des points aux clusters en parallèle
    #pragma omp parallel
    {
        int* local_sizes = (int*)calloc(k, sizeof(int));
        double* local_sums_x = (double*)calloc(k, sizeof(double));
        double* local_sums_y = (double*)calloc(k, sizeof(double));

        #pragma omp for
        for (int i = 0; i < n; i++) {
            int cluster_id = closest_cluster(points[i], clusters, k);
            points[i].cluster_id = cluster_id;

            // Mise à jour locale des centroids
            local_sizes[cluster_id]++;
            local_sums_x[cluster_id] += points[i].x;
            local_sums_y[cluster_id] += points[i].y;
        }

        // Réduction pour mettre à jour les clusters globaux
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

// Mise à jour des centroids (inchangée)
void update_centroids(Cluster* clusters, int k) {
    for (int i = 0; i < k; i++) {
        if (clusters[i].size > 0) {
            clusters[i].centroid_x = clusters[i].new_centroid_x / clusters[i].size;
            clusters[i].centroid_y = clusters[i].new_centroid_y / clusters[i].size;
        }
    }
}

// Fonction principale de k-means
void kmeans(Point* points, Cluster* clusters, int n, int k, int iterations) {
    initialize_clusters(clusters, points, n, k);

    for (int iter = 0; iter < iterations; iter++) {
        assign_clusters(points, clusters, n, k);
        update_centroids(clusters, k);
    }
}
