// This program finds max flow in undirected graph
// We use Push & Relable of preflow algorithm
// O(V^2 * E)

#include <iostream>
#include "flow_network.h"

int main() {
	while (true) {
		int vertices_quantity;// quantity of vertices in graph
		std::cin >> vertices_quantity;
		if (vertices_quantity == 0) {
			break;
		}
		int source_number;// number of source
		int target_number;// number of target
		int edges_quantity;// quantity of edges in graph
		std::cin >> source_number >> target_number >> edges_quantity;

		FlowNetwork net(vertices_quantity, source_number - 1, target_number - 1);
		int from_number;// number of vertex, which is begin of edge
		int to_number; // number of vertex, which is end of edge
		int capacity; // capacity of flow throught this edge
		for (int i = 0; i < edges_quantity; ++i) {
			std::cin >> from_number >> to_number >> capacity;
			net.AddEdge(from_number - 1, to_number - 1, capacity);
		}
		std::cout << net.FindMaxFlowByPushingOfPreflow() << std::endl;
	}


	return 0;
}