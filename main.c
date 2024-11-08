#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "./struct.h"
#include "./kmeans_sequential.h"

#define NUM_POINTS 1000
#define NUM_CLUSTERS 3
#define NUM_ITERATIONS 10

int main() {
    Point points[NUM_POINTS];
    Cluster clusters[NUM_CLUSTERS];

    srand(0); // Pour reproductibilité
    initialize_points(points, NUM_POINTS);
    
    kmeans(points, clusters, NUM_POINTS, NUM_CLUSTERS, NUM_ITERATIONS);

    // Affiche les centroids finaux
    for (int i = 0; i < NUM_CLUSTERS; i++) {
        printf("Cluster %d: Centroid at (%.3f, %.3f) with %d points\n", 
               i, clusters[i].centroid_x, clusters[i].centroid_y, clusters[i].size);
    }

    return 0;
}