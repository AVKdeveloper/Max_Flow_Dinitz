#include "flow_network.h"

FlowNetwork::FlowNetwork(const int& vertices_quentity,
	const int& source_number, const int& target_number) {
	vertices_quentity_ = vertices_quentity;
	source_number_ = source_number;
	target_number_ = target_number;
	edges_.resize(vertices_quentity_);
	height_.resize(vertices_quentity_);
	levels_.resize(vertices_quentity_);
	excess_flow_.resize(vertices_quentity_);
	for (int i = 0; i < vertices_quentity_; ++i) {
		edges_[i].resize(vertices_quentity_);
		for (int j = 0; j < vertices_quentity_; ++j) {
			edges_[i][j].existing_ = false;
			edges_[i][j].capacity_remained_ = 0;
		}
	}
}

void FlowNetwork::AddEdge(const int& from, const int& to, const int& capacity) {
	if (!edges_[from][to].existing_) {
		edges_[from][to].existing_ = true;
		edges_[from][to].capacity_remained_ = capacity;
		edges_[from][to].flow_ = 0;
		edges_[to][from].existing_ = true;
		edges_[to][from].capacity_remained_ = capacity;
		edges_[to][from].flow_ = 0;
	}
	else { //if this edge already exists we can only increase the capacity
		edges_[from][to].capacity_remained_ += capacity;
		edges_[to][from].capacity_remained_ += capacity;
	}
}

bool FlowNetwork::Push(const int& from, const int& to) {
	if (excess_flow_[from] > 0 && edges_[from][to].existing_ && height_[from] == height_[to] + 1 &&
		edges_[from][to].capacity_remained_ > 0) {
		int delta_of_pushing = std::min(excess_flow_[from], edges_[from][to].capacity_remained_);
		excess_flow_[from] -= delta_of_pushing;
		excess_flow_[to] += delta_of_pushing;
		edges_[from][to].flow_ += delta_of_pushing;
		edges_[from][to].capacity_remained_ -= delta_of_pushing;
		edges_[to][from].flow_ -= delta_of_pushing;
		edges_[to][from].capacity_remained_ += delta_of_pushing;
		return true;
	}
	else {
		return false;
	}
}

bool FlowNetwork::Relabel(const int& vertex) {
	if (vertex != source_number_ && vertex != target_number_ && excess_flow_[vertex] > 0) {
		int min_neihbour_height = 2 * height_[vertex];// there are theorem that for every vertex height[vertex] < 2V-2
		for (int i = 0; i < vertices_quentity_; ++i) {
			if (edges_[vertex][i].existing_ && edges_[vertex][i].capacity_remained_ > 0 &&
				min_neihbour_height > height_[i]) {
				min_neihbour_height = height_[i];
			}
		}
		if (height_[vertex] <= min_neihbour_height) {
			height_[vertex] = min_neihbour_height + 1;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void FlowNetwork::InitializePreflow() {
	for (int i = 0; i < vertices_quentity_; ++i) {
		height_[i] = 0;
		excess_flow_[i] = 0;
		for (int j = 0; j < vertices_quentity_; ++j) {
			edges_[i][j].flow_ = 0;
		}
	}
	height_[source_number_] = vertices_quentity_;
	for (int i = 0; i < vertices_quentity_; ++i) {
		if (edges_[source_number_][i].existing_) {
			edges_[source_number_][i].flow_ = edges_[source_number_][i].capacity_remained_;
			edges_[i][source_number_].flow_ = -edges_[source_number_][i].capacity_remained_;
			edges_[source_number_][i].capacity_remained_ = 0;
			edges_[i][source_number_].capacity_remained_ *= 2;
			excess_flow_[i] = edges_[source_number_][i].flow_;
			excess_flow_[source_number_] -= edges_[source_number_][i].flow_;
		}
	}
}

void FlowNetwork::PushRelabelOfPreflow() {
	InitializePreflow();
	bool done_operations = true;// indicator that we have done at least one operation
	while (done_operations) { // estimation: ~E times
		done_operations = false;
		for (int i = 0; i < vertices_quentity_; ++i) {// O(V)
			if (excess_flow_[i] > 0) {
				done_operations |= Relabel(i); // O(V)
				for (int j = 0; j < vertices_quentity_; ++j) { // O(V)
					done_operations |= Push(i, j);
				}
			}
		}
	}
}

int FlowNetwork::FindMaxFlowByPushingOfPreflow() {
	PushRelabelOfPreflow();
	if (excess_flow_[source_number_] == -excess_flow_[target_number_]) {
		return excess_flow_[target_number_];
	}
	else {
		return -1;
	}
}

bool FlowNetwork::AssignLevels() {
	// this method sets levels to every vertex in remained network using bfs
	// returns true if we have a way from the source to the target in remained network
	std::vector<bool> visited_vertices(vertices_quentity_); // false means we haven't visited this vertex
	std::fill(visited_vertices.begin(), visited_vertices.end(), false);
	for (int i = 0; i < vertices_quentity_; ++i) { 
		levels_[i] = -1; // "-1" means we haven't reached this vertex
	}
	levels_[source_number_] = 0;
	std::queue<int> queue_of_vertices;
	queue_of_vertices.push(source_number_);
	while (queue_of_vertices.size() > 0) {
		int current_vertex = queue_of_vertices.front();
		queue_of_vertices.pop();
		if (visited_vertices[current_vertex]) { // if we have already visited this vertex
			continue;
		} else {
			visited_vertices[current_vertex] = true;
			for (int next_vertex = 0; next_vertex < vertices_quentity_; ++next_vertex) {
				if ( !visited_vertices[next_vertex] && edges_[current_vertex][next_vertex].capacity_remained_ > 0) { 
					// if we have opportunity to get into the unvisited next_vertex
					levels_[next_vertex] = levels_[current_vertex] + 1;
					queue_of_vertices.push(next_vertex);
				}
			}
		}
	}
	return levels_[target_number_] > 0; 
}

int FlowNetwork::DfsInLayeredNetwork(const int& vertex_from, const int flow, std::vector<int>& possible_next_vertices) {
	// possible_next_vertices[i] - min No of next vertex, which we can reach target through
	if (flow == 0) {
		return 0;
	} else if (vertex_from == target_number_) {
		return flow;
	} else {
		for (int vertex_to = possible_next_vertices[vertex_from]; vertex_to < vertices_quentity_; ++vertex_to) {
			if (levels_[vertex_to] != levels_[vertex_from] + 1) {
				possible_next_vertices[vertex_from] += 1;
				continue;
			}
			int delta_of_pushing = DfsInLayeredNetwork(vertex_to, std::min(flow, edges_[vertex_from][vertex_to].capacity_remained_), possible_next_vertices);
			if (delta_of_pushing > 0) {
				edges_[vertex_from][vertex_to].flow_ += delta_of_pushing;
				edges_[vertex_from][vertex_to].capacity_remained_ -= delta_of_pushing;
				edges_[vertex_to][vertex_from].flow_ -= delta_of_pushing;
				edges_[vertex_to][vertex_from].capacity_remained_ += delta_of_pushing;
				return delta_of_pushing;
			} else {
				possible_next_vertices[vertex_from] += 1;
			}
		}
		return 0;
	}
}

int FlowNetwork::FindMaxFlowByDinitz() {
	int result_flow = 0;
	while (true) {
		if (!AssignLevels()) {
			break;
		} else {
			std::vector<int> possible_next_vertices(vertices_quentity_);
			// possible_next_vertices[i] - min No of next vertex, which we can reach target through
			std::fill(possible_next_vertices.begin(), possible_next_vertices.end(), 0);
			while (int delta_of_pushing = DfsInLayeredNetwork(source_number_, std::numeric_limits<int>::max(), possible_next_vertices)) {
				result_flow += delta_of_pushing;
			}
		}
	}
	return result_flow;
}