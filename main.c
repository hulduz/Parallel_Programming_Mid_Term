#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <Python.h>
#include "./struct.h"
#include "./kmeans_sequential.h"
#include "./kmeans_parallel.h"

#define NUM_POINTS 1000
#define NUM_CLUSTERS 3
#define NUM_ITERATIONS 10

void initialize_points(Point* points, int n) {
    srand(0); 
    for (int i = 0; i < n; i++) {
        points[i].x = rand() / (double)RAND_MAX;
        points[i].y = rand() / (double)RAND_MAX;
        points[i].cluster_id = -1; // Initially, no cluster assigned
    }
}

void export_data(Point* data, int* labels, double** centroids, int num_points, int num_clusters, const char* prefix) {
    char data_filename[100], labels_filename[100], centroids_filename[100];
    sprintf(data_filename, "%s_data.txt", prefix);
    sprintf(labels_filename, "%s_labels.txt", prefix);
    sprintf(centroids_filename, "%s_centroids.txt", prefix);

    FILE* data_file = fopen(data_filename, "w");
    FILE* labels_file = fopen(labels_filename, "w");
    FILE* centroids_file = fopen(centroids_filename, "w");

    if (data_file == NULL || labels_file == NULL || centroids_file == NULL) {
        printf("Erreur lors de l'ouverture des fichiers !\n");
        return;
    }

    // To save the points
    for (int i = 0; i < num_points; i++) {
        fprintf(data_file, "%f %f\n", data[i].x, data[i].y);
    }
    fclose(data_file);

    // To save the labels
    for (int i = 0; i < num_points; i++) {
        fprintf(labels_file, "%d\n", labels[i]);
    }
    fclose(labels_file);

    // To save the centroids
    for (int i = 0; i < num_clusters; i++) {
        fprintf(centroids_file, "%f %f\n", centroids[i][0], centroids[i][1]);
    }
    fclose(centroids_file);
}

void run_python_script() {
    Py_Initialize();

    // Python script to visualize the centroids
    const char* code =
        "import numpy as np\n"
        "import matplotlib.pyplot as plt\n"
        "data_seq = np.loadtxt('seq_data.txt')\n"
        "labels_seq = np.loadtxt('seq_labels.txt', dtype=int)\n"
        "centroids_seq = np.loadtxt('seq_centroids.txt')\n"
        "data_par = np.loadtxt('par_data.txt')\n"
        "labels_par = np.loadtxt('par_labels.txt', dtype=int)\n"
        "centroids_par = np.loadtxt('par_centroids.txt')\n"
        
        // Creating the visualization
        "plt.figure(figsize=(8, 6))\n"
        "for i in range(centroids_seq.shape[0]):\n"
        "    cluster_points = data_seq[labels_seq == i]\n"
        "    plt.scatter(cluster_points[:, 0], cluster_points[:, 1], label=f'Seq Cluster {i}')\n"
        "plt.scatter(centroids_seq[:, 0], centroids_seq[:, 1], color='red', marker='X', s=100, label='Seq Centroids')\n"
        "for i in range(centroids_par.shape[0]):\n"
        "    cluster_points = data_par[labels_par == i]\n"
        "    plt.scatter(cluster_points[:, 0], cluster_points[:, 1], label=f'Par Cluster {i}', alpha=0.5)\n"
        "plt.scatter(centroids_par[:, 0], centroids_par[:, 1], color='black', marker='X', s=100, label='Par Centroids')\n"
        
        "plt.xlabel('Feature 1')\n"
        "plt.ylabel('Feature 2')\n"
        "plt.title('Clusters Visualization (Sequential vs Parallel)')\n"
        "plt.legend()\n"
        "plt.show()\n";

    PyRun_SimpleString(code);

    Py_Finalize();
}

int main() {
    remove("data.txt");
    remove("labels.txt");
    remove("centroids.txt");


    Point points_seq[NUM_POINTS], points_par[NUM_POINTS];
    Cluster clusters_seq[NUM_CLUSTERS], clusters_par[NUM_CLUSTERS];

    srand(0); 
    initialize_points(points_seq, NUM_POINTS);

    // Copy the points for both versions
    for (int i = 0; i < NUM_POINTS; i++) {
        points_par[i] = points_seq[i];
    }

    printf("Running sequential version of k-means...\n");
    double start_seq = omp_get_wtime();
    kmeans(points_seq, clusters_seq, NUM_POINTS, NUM_CLUSTERS, NUM_ITERATIONS);
    double end_seq = omp_get_wtime();
    printf("Sequential version completed in %.3f seconds.\n", end_seq - start_seq);

    printf("\nRunning parallel version of k-means...\n");
    double start_par = omp_get_wtime();
    kmeans_parallel(points_par, clusters_par, NUM_POINTS, NUM_CLUSTERS, NUM_ITERATIONS);
    double end_par = omp_get_wtime();
    printf("Parallel version completed in %.3f seconds.\n", end_par - start_par);

    // Creating a board for labels and centroids for the exportation 
    int* labels_seq = (int*)malloc(NUM_POINTS * sizeof(int));
    int* labels_par = (int*)malloc(NUM_POINTS * sizeof(int));
    for (int i = 0; i < NUM_POINTS; i++) {
        labels_seq[i] = points_seq[i].cluster_id;
        labels_par[i] = points_par[i].cluster_id;
    }

    // Creating a board for centroids 
    double** centroids_seq = (double**)malloc(NUM_CLUSTERS * sizeof(double*));
    double** centroids_par = (double**)malloc(NUM_CLUSTERS * sizeof(double*));
    for (int i = 0; i < NUM_CLUSTERS; i++) {
        centroids_seq[i] = (double*)malloc(2 * sizeof(double));
        centroids_par[i] = (double*)malloc(2 * sizeof(double));
        centroids_seq[i][0] = clusters_seq[i].centroid_x;
        centroids_seq[i][1] = clusters_seq[i].centroid_y;
        centroids_par[i][0] = clusters_par[i].centroid_x;
        centroids_par[i][1] = clusters_par[i].centroid_y;
    }

    // Exporting the results of both versions
    export_data(points_seq, labels_seq, centroids_seq, NUM_POINTS, NUM_CLUSTERS, "seq");
    export_data(points_par, labels_par, centroids_par, NUM_POINTS, NUM_CLUSTERS, "par");

    // Memory liberation after exportation
    free(labels_seq);
    free(labels_par);
    for (int i = 0; i < NUM_CLUSTERS; i++) {
        free(centroids_seq[i]);
        free(centroids_par[i]);
    }
    free(centroids_seq);
    free(centroids_par);

    run_python_script();

    return 0;
}
