#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <chrono>
#include "SimpleGraph.h"

using namespace std;

void Welcome();
int getInteger();
void LoadGraphFromFile(SimpleGraph&, ifstream&);
void InitNodesPosition(SimpleGraph&);
void UpdateGraph(SimpleGraph&);

constexpr double PI = 3.14159265358979323;
constexpr double K_ATTACT = 0.001;
constexpr double K_REPEL = 1e-3;

// Main method
int main() {
    Welcome();

    string filename;
    ifstream file;
    do {
        cout << "Filename of graph to visualize: ";
        getline(cin, filename);
        file.open(filename);
    } while (!file.is_open());

    int duration = getInteger();

    SimpleGraph graph;
    LoadGraphFromFile(graph, file);

    InitNodesPosition(graph);

    InitGraphVisualizer(graph);
    DrawGraph(graph);

    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end-start);
    while (elapsed.count() < duration) {
        UpdateGraph(graph);
        DrawGraph(graph);

        end = chrono::high_resolution_clock::now();
        elapsed = chrono::duration_cast<chrono::milliseconds>(end-start);
    }

    return 0;
}

void UpdateGraph(SimpleGraph& graph) {
    vector<Node> deltas(graph.nodes.size());

    // attractive force between the endpoints of each edge
    for (auto edge: graph.edges) {
        auto [x0, y0] = graph.nodes[edge.start];
        auto [x1, y1] = graph.nodes[edge.end];

        double fAttact = K_ATTACT * ((y1-y0)*(y1-y0) + (x1-x0)*(x1-x0));
        double theta = atan2(y1-y0, x1-x0);

        deltas[edge.start].x += fAttact * cos(theta);
        deltas[edge.start].y += fAttact * sin(theta);
        deltas[edge.end].x -= fAttact * cos(theta);
        deltas[edge.end].y -= fAttact * sin(theta);
    }

    // repulsive force between every pair of nodes.
    for (size_t i = 0; i < graph.nodes.size(); i++) {
        for (size_t j = i+1; j < graph.nodes.size(); j++) {
            auto [x0, y0] = graph.nodes[i];
            auto [x1, y1] = graph.nodes[2];

            double fRepel = K_REPEL / sqrt((y1-y0)*(y1-y0) + (x1-x0)*(x1-x0));
            double theta = atan2(y1-y0, x1-x0);

            deltas[i].x -= fRepel * cos(theta);
            deltas[i].y -= fRepel * sin(theta);
            deltas[j].x += fRepel * cos(theta);
            deltas[j].y += fRepel * sin(theta);
        }
    }

    for (size_t i = 0; i < graph.nodes.size(); i++) {
        graph.nodes[i].x += deltas[i].x;
        graph.nodes[i].y += deltas[i].y;
    }
}

void InitNodesPosition(SimpleGraph& graph) {
    size_t n = graph.nodes.size();
    for (size_t i = 0; i < n; i++) {
        graph.nodes[i].x = cos(2 * PI * i / n);
        graph.nodes[i].y = sin(2 * PI * i / n);
    }
}

void LoadGraphFromFile(SimpleGraph& graph, ifstream& file) {
    size_t n;
    file >> n;
    graph.nodes.resize(n);

    size_t n1, n2;
    while (file >> n1 >> n2)
        graph.edges.push_back(Edge{n1, n2});
}

/* Get a positive integer from standard input and return to
 * caller. */
int getInteger() {
    while (true) {
        cout << "Algorithm runtime duration in ms: ";
        string line; int result; char trash;
        if (!getline(cin, line)) continue;

        istringstream iss(line);
        if (iss >> result && !(iss >> trash) && result > 0) return result;
    }
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}


