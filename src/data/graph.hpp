#ifndef GRAPH_HPP_INCLUDED
#define GRAPH_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <cctype>
#include <limits>

struct old_graph_t
{
    int vertex_num;
    int edge_num;
    int * vertex_begin;
    int * edge_dest;
    int * edge_src;
    int * vertex_end;
};

template<typename NodeType, typename EdgeType>
struct graph_t {
	// number of nodes
	NodeType node_num{};
	// number of edges
	EdgeType edge_num{};
	// pointer for each row (edge index for each node), of size node_num+1
	EdgeType * row_ptr{};
	// col index (target node) for each edge, of size edge_num
	NodeType * col_ind{};
	// row index (source node) for each edge, of size edge_num
	NodeType * row_ind{};

	// allocate memory storage
	void allocate(NodeType _node_num, EdgeType _edge_num) {
		node_num = _node_num;
		edge_num = _edge_num;
		row_ptr = new EdgeType[node_num + 1]{};
		col_ind = new NodeType[edge_num]{};
		row_ind = new NodeType[edge_num]{};
	}
	// release memory storage
	void release() {
		node_num = 0;
		edge_num = 0;
		delete[] row_ptr;
		delete[] col_ind;
		delete[] row_ind;
		row_ptr = nullptr;
		col_ind = nullptr;
		row_ind = nullptr;
	}

	// write data into a binary file
	bool write_bin_file(char const * filename) {
		std::ofstream file(filename, std::ios::binary);
		if (!file.is_open()) return false;
		// type information
		// int s1 = sizeof(NodeType);
		int s1 = sizeof(struct old_graph_t);   // tricky
		file.write((char*)&s1, sizeof(int));
		// int s2 = sizeof(EdgeType);
		int s2 = sizeof(int);   // tricky
		file.write((char*)&s2, sizeof(int));
		// graph size
		file.write((char*)&node_num, sizeof(NodeType));
		file.write((char*)&edge_num, sizeof(EdgeType));
		// graph data
		file.write((char*)row_ptr, sizeof(EdgeType) * (node_num + 1));
		file.write((char*)row_ind, sizeof(NodeType) * edge_num);
		file.write((char*)col_ind, sizeof(NodeType) * edge_num);
		// done
		return true;
	}
	// read data from a binary file
	bool read_bin_file(char const * filename) {
		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open()) return false;
		// type information
		int s1 = 0, s2 = 0;
		file.read((char*)&s1, sizeof(int));
		file.read((char*)&s2, sizeof(int));
		if (s1 != sizeof(NodeType) || s2 != sizeof(EdgeType))
			std::cerr << "Possible Unknown File Format !" << std::endl;
		// graph size
		NodeType _node_num{};
		EdgeType _edge_num{};
		file.read((char*)&_node_num, sizeof(NodeType));
		file.read((char*)&_edge_num, sizeof(EdgeType));
		if (_node_num <= 0 || _edge_num <= 0)
			return false;
		allocate(_node_num, _edge_num);
		// graph data
		file.read((char*)row_ptr, sizeof(EdgeType) * (node_num + 1));
		if (file.gcount() == sizeof(EdgeType) * (node_num + 1)) {
			file.read((char*) row_ind, sizeof(NodeType) * edge_num);
			if (file.gcount() == sizeof(NodeType) * edge_num) {
				file.read((char*) col_ind, sizeof(NodeType) * edge_num);
				if (file.gcount() == sizeof(NodeType) * edge_num) {
					return true;
				}
			}
		}
		return false;
	}

	// write data into a text file
	bool write_txt_file(char const * filename) {
		std::ofstream file(filename);
		if (!file.is_open()) return false;
		file << "#Nodes #Edges\n";
		file << node_num << " " << edge_num;
		file << "\n#Source #Destination(s)\n";
		for (NodeType node = 0; node < node_num; ++node) {
			EdgeType e1 = row_ptr[node];
			EdgeType e2 = row_ptr[node + 1];
			if (e2 > e1) {
				file << node;
				for (EdgeType e = e1; e < e2; ++e)
					file << ' ' << col_ind[e];
				file << '\n';
			}
		}
		return true;
	}
	// read data from a text file
	bool read_txt_file(char const * filename) {
		std::ifstream file(filename);
		if (!file.is_open()) return false;
		// skip non-digit-leading lines
		while(!file.eof()) {
			int c = file.peek();
			if (isdigit(c)) break;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		// graph size
		NodeType _node_num{};
		EdgeType _edge_num{};
		file >> _node_num >> _edge_num;
		if (_node_num <= 0 || _edge_num <= 0)
			return false;
		allocate(_node_num, _edge_num);
		// graph data
		NodeType curNode{}, prevNode{};
		EdgeType edgeSum{};
		bool new_line = false;
		while (!file.eof() && edgeSum < edge_num) {
			int c = file.peek();
			if (isspace(c)) {
				if (c == '\n') new_line = true;
				file.ignore();
			}
			else if (isdigit(c)) {
				file >> curNode;
				if (new_line) {
					// the source node ID
					new_line = false;
					if (curNode != prevNode) {
						for (NodeType n = prevNode + 1; n <= curNode; ++n)
							row_ptr[n] = edgeSum;
						prevNode = curNode;
					}
				} else {
					// the destination node ID
					col_ind[edgeSum] = curNode;
					row_ind[edgeSum] = prevNode;
					edgeSum += 1;
				}
			} else {
				// skip the (rest of the) line if met a non-didit char
				file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		for (NodeType n = prevNode + 1; n <= node_num; ++n)
			row_ptr[n] = edgeSum;
		return (edgeSum == edge_num);
	}

	// write data into a text file
	bool write_mtx_file(char const * filename) {
		std::ofstream file(filename);
		if (!file.is_open()) return false;
		file << "%%MatrixMarket matrix coordinate real general\n";
		file << node_num << ' ' << node_num << ' ' << edge_num << '\n';
		EdgeType * in_degree = new EdgeType[node_num]{};
		NodeType * col = new NodeType[edge_num];
		NodeType * row = new NodeType[edge_num];
		for (EdgeType e = 0; e < edge_num; ++e) {
			in_degree[col_ind[e]] += 1;
		}
		EdgeType sum = 0;
		for (NodeType n = 0; n < node_num; ++n) {
			EdgeType tmp = sum;
			sum += in_degree[n];
			in_degree[n] = tmp;
		}
		for (EdgeType e = 0; e < edge_num; ++e) {
			NodeType src = row_ind[e];
			NodeType dest = col_ind[e];
			EdgeType pos = in_degree[dest];
			in_degree[dest] += 1;
			col[pos] = dest + 1 ;
			row[pos] = src + 1;
		}
		for (EdgeType e = 0; e < edge_num; ++e) {
			file << row[e] << ' ' << col[e] << '\n';
		}
		delete[] in_degree;
		delete[] row;
		delete[] col;
		return true;
	}
	// read data from a text file
	bool read_mtx_file(char const * filename) {
		std::ifstream file(filename);
		if (!file.is_open()) return false;
		// skip non-digit-leading lines
		while (!file.eof()) {
			int c = file.peek();
			if (isdigit(c)) break;
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		// graph size
		NodeType _node_num{}, _node_num2{};
		EdgeType _edge_num{};
		file >> _node_num >> _node_num2 >> _edge_num;
		if (_node_num <= 0 || _node_num2 <= 0 || _edge_num <= 0)
			return false;
		allocate(_node_num, _edge_num);
		EdgeType * off = row_ptr + 1;
		NodeType * col = new NodeType[edge_num] {};
		NodeType * row = new NodeType[edge_num] {};
		// graph data
		EdgeType edgeSum{};
		while (!file.eof() && edgeSum < edge_num) {
			int c = file.peek();
			if (isspace(c)) {
				file.ignore();
			} else {
				if (isdigit(c)) {
					// the source & destination node ID
					NodeType src, dest;
					file >> src >> dest;
					row[edgeSum] = src - 1;
					col[edgeSum] = dest - 1;
					edgeSum += 1;
					// count out degree
					off[src - 1] += 1;
				}
				// ignore other contents
				file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		// calculate offset
		EdgeType sum = 0;
		for (NodeType v = 0; v < node_num; ++v) {
			EdgeType tmp = sum;
			sum += off[v];
			off[v] = tmp;
		}
		// copy result
		for (EdgeType e = 0; e < edgeSum; ++e) {
			NodeType src_id = row[e];
			NodeType dest_id = col[e];
			EdgeType pos = off[src_id]++;
			col_ind[pos] = dest_id;
			row_ind[pos] = src_id;
		}
		delete[] row;
		delete[] col;
		return (edgeSum == edge_num);
	}
};

#endif // GRAPH_HPP_INCLUDED
