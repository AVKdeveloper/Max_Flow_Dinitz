#ifndef FLOW_NETWORK
#define FLOW_NETWORK

#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

struct Edge {
	bool existing_;
	int capacity_remained_;
	int flow_;
};

class FlowNetwork {
	int vertices_quentity_;
	int source_number_;
	int target_number_;
	std::vector<std::vector<Edge>> edges_;
	std::vector<int> excess_flow_; // for push-relabel
	std::vector<int> height_; // for push-relabel
	std::vector<int> levels_; // for Dinitza
public:
	FlowNetwork(const int& vertices_quentity, const int& source_number, const int& target_number);
	void AddEdge(const int& from, const int& to, const int& capacity);
	bool Push(const int& from, const int& to); // push of flow
	bool Relabel(const int& vertex);
	void InitializePreflow();
	void PushRelabelOfPreflow();
	int FindMaxFlowByPushingOfPreflow();

	bool AssignLevels();
	int DfsInLayeredNetwork(const int& vertex_from, const int flow, std::vector<int> possible_next_vertices);
	int FindMaxFlowByDinitz();
};


#endif
