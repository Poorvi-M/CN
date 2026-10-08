#include <iostream>
#include <vector>
#include <climits>

using namespace std;

// Structure to represent a weighted edge
struct Edge {
    int src;
    int dest;
    int weight;
};

// Bellman-Ford Algorithm
void bellmanFord(int V, int E, const vector<Edge>& edges, int src) {

    // Step 1: Initialize distances
    vector<int> dist(V, INT_MAX);

    // Distance from source to itself is 0
    dist[src] = 0;

    // Step 2: Relax all edges V-1 times
    for (int i = 1; i <= V - 1; ++i) {

        for (int j = 0; j < E; ++j) {

            int u = edges[j].src;
            int v = edges[j].dest;
            int weight = edges[j].weight;

            // Relax the edge
            if (dist[u] != INT_MAX &&
                dist[u] + weight < dist[v]) {

                dist[v] = dist[u] + weight;
            }
        }
    }

    // Step 3: Check for negative-weight cycles
    bool hasNegativeCycle = false;

    for (int j = 0; j < E; ++j) {

        int u = edges[j].src;
        int v = edges[j].dest;
        int weight = edges[j].weight;

        // If distance can still be reduced,
        // a negative-weight cycle exists
        if (dist[u] != INT_MAX &&
            dist[u] + weight < dist[v]) {

            hasNegativeCycle = true;
            break;
        }
    }

    // Step 4: Print the result
    if (hasNegativeCycle) {

        cout << "\nGraph contains a negative weight cycle!"
             << endl;

        cout << "Shortest paths cannot be uniquely determined."
             << endl;
    }
    else {

        cout << "\nVertex Distance from Source ("
             << src << "):" << endl;

        cout << "Vertex\tDistance" << endl;

        for (int i = 0; i < V; ++i) {

            if (dist[i] == INT_MAX) {
                cout << i << "\tINF" << endl;
            }
            else {
                cout << i << "\t"
                     << dist[i] << endl;
            }
        }
    }
}

int main() {

    int V, E;

    // Input number of vertices
    cout << "Enter the number of vertices: ";
    cin >> V;

    // Input number of edges
    cout << "Enter the number of edges: ";
    cin >> E;

    // Create vector to store all edges
    vector<Edge> edges(E);

    // Input edge details
    cout << "Enter edges details "
         << "(source, destination, weight):"
         << endl;

    for (int i = 0; i < E; ++i) {

        cin >> edges[i].src
            >> edges[i].dest
            >> edges[i].weight;
    }

    // Input source vertex
    int source;

    cout << "Enter the source vertex: ";
    cin >> source;

    // Run Bellman-Ford
    bellmanFord(V, E, edges, source);

    return 0;
}
