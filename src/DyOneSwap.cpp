#include "DyOneSwap.h"

void Node::_insert_edge(const int& id) {
    degree ++;
    edges.push_back(id);
}

void Node::_delete_edge(const int& id) {
    for (int i = 0; i < degree; ++ i) {
        if (edges[i] == id) {
            edges[i] = edges.back();
            edges.pop_back();
            break;
        }
    }
    degree --;
}

void Node::_insert_L(const int& id) {
    count ++;
    L[id] = 1;
}

void Node::_delete_L(const int& id) {
    count --;
    L.erase(id);
}

Graph::Graph() {
    n = m = mis = 0;
    utime = 0;
}

Graph::~Graph() {}
    
void Graph::load_graph(const char* path) {
    ifstream infile((_GRAPH_PATH + string(path)).c_str());
    if (!infile.is_open()) {
        cout << "graph file: " << path << " open failed." << endl;
        exit(1);
    }
    int u, v;
    infile >> n >> m;
    nodes.resize(n);
    for (int i = 0; i < n; ++ i) nodes[i].id = i;
    for (int i = 0; i < m; ++ i) {
        infile >> u >> v;
        nodes[u]._insert_edge(v);
        nodes[v]._insert_edge(u);
    }
    infile.close();
}

void Graph::load_mis(const char* path) {
    ifstream infile((_MIS_PATH + string(path)).c_str());
    if (!infile.is_open()) {
        cout << "mis file: " << path << " open failed." << endl;
        exit(1);
    }
    int id;
    infile >> mis;
    for (int i = 0; i < mis; ++ i) {
        infile >> id;
        nodes[id].status = 1;
        for (int u : nodes[id].edges) {
            nodes[u].status = 2;
            if (nodes[u].count == 0) nodes[id]._insert_L(u);
            else if (nodes[u].count == 1) {
                int v = nodes[u].L.begin()->first;
                nodes[v]._delete_L(u);
            }
            nodes[u]._insert_L(id);
        }
    }
    for (int i = 0; i < n; ++ i) {
        if (nodes[i].status == 0 && nodes[i].edges.size() == 0) {
            nodes[i].status = 1;
            mis ++;
        }
    }
    infile.close();
}
 
void Graph::eff_exp(const char* path, const int& num) {
    ifstream infile ((_INST_PATH + string(path)).c_str());
    if (!infile.is_open()) {
        cout << "instruction file: " << path << " open failed." << endl;
        exit(1);
    }
    for (int i = 0; i < num; ++ i) {
        Update op;
        infile >> op.type;
        if (op.type == 0) {
            infile >> op.u >> op.v;
            _insert_edge(op.u, op.v);
        } else if (op.type == 1) {
            infile >> op.u >> op.v;
            _delete_edge(op.u, op.v);
        } else if (op.type == 2) {
            infile >> op.u;
            _delete_vertex(op.u);
        }
    }
    cout << "DyOneSwap\t" << string(path) << "\tutime = " << utime << "\t|V| = " << n << "\t|I| = " << mis << endl;
    infile.close();
}

void Graph::check() {
    int check_mis = 0;
    bool maximal = true;
    for (auto node : nodes) {
        if (node.status == 1) {
            int check_count = 0;
            check_mis ++;
            for (int w : node.edges) {
                if (nodes[w].status == 1) {
                    cout << "WA: two adjacent vertices in I.\n";
                    exit(1);
                } else if (nodes[w].status == 2 && nodes[w].count == 1) {
                    check_count ++;
                }
            }
            if (check_count != node.count) {
                cout << "WA: " << node.id << '\t' << node.status << " wrong count. " << check_count << '\t' << node.count << endl;
                exit(1);
            }
            for (auto w : node.L) {
                if (nodes[w.first].count != 1 || nodes[w.first].status == 1) {
                    cout << "WA: " << node.id << " wrong L list. " << w.first << '\t' << nodes[w.first].status << '\t' << nodes[w.first].count << endl;
                    exit(1);
                }
                int cap = 0;
                for (int id : nodes[w.first].edges)
                    if (node.L.count(id) != 0) cap ++;
                if (cap + 1 < node.count) {
                    cout << "WA: " << node.id << " not 1-maximal.\n";
                    exit(1);
                }
            }
        } else if (node.status == 2) {
            int check_count = 0;
            bool found = false;
            for (int w : node.edges) {
                if (nodes[w].status == 1) {
                    found = true;
                    check_count ++;
                }
            }
            if (!found) {
                cout << "WA: not maximal. " << node.id << '\t' << node.status << endl;
                exit(1);
            }
            if (check_count != node.count) {
                cout << "WA: " << node.id << '\t' << node.status << " wrong count. " << check_count << '\t' << node.count << endl;
                cout << "L :\n";
                for (auto iter : node.L) cout << iter.first << endl;
                cout << "N :\n";
                for (int w : node.edges) {
                    if (nodes[w].status == 1) {
                        cout << w << endl;
                    }
                }
                exit(1);
            }
        } else if (node.status == 0) {
            cout << "WA: not maximal. " << node.id << '\t' << node.status << endl;
            exit(1);
        }
    }
    if (check_mis != mis) {
        cout << "WA: wrong mis counter.\n";
        exit(1);
    }
}

void Graph::_insert_candidate(const int& u, const int& v) {
    if (candidates.count(u) == 0) {
        vector<int> C; C.push_back(v); candidates[u] = C;
    } else candidates[u].push_back(v);
}

void Graph::_delete_vertex(const int& id) {
#ifdef _LINUX_
    struct timeval start, end;
    gettimeofday(&start, NULL);
#endif
    n --;
    for (int u : nodes[id].edges) nodes[u]._delete_edge(id);
    if (nodes[id].status == 1) {
        int u = id;
        if (nodes[id].count > 0) u = nodes[id].L.begin()->first;
        _move_out(id);
        if (u != id) _move_in(u);
        for (int v : nodes[id].edges)
            if (nodes[v].status == 2 && nodes[v].count == 1)
                _insert_candidate(nodes[v].L.begin()->first, v);
        if (!candidates.empty()) _extend_oneswap();
    } else if (nodes[id].status == 2) {
        if (nodes[id].count == 1)
            nodes[nodes[id].L.begin()->first]._delete_L(id);
    }
    nodes[id].status = -1;
    nodes[id].edges.clear();
    nodes[id].L.clear();
#ifdef _LINUX_
    gettimeofday(&end, NULL);
    long long mtime, seconds, useconds;
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = seconds*1000000 + useconds;
    utime += mtime;
#endif
}

void Graph::_insert_edge(int u, int v) {
#ifdef _LINUX_
    struct timeval start, end;
    gettimeofday(&start, NULL);
#endif
    m ++;
    nodes[u]._insert_edge(v);
    nodes[v]._insert_edge(u);
    if (nodes[u].status == 1 && nodes[v].status == 1) {
        if (nodes[u].count == 0 && nodes[v].count == 0) {
            if (nodes[u].degree < nodes[v].degree) swap(u, v);
            _move_out(u);
            nodes[u]._insert_L(v);
            nodes[v]._insert_L(u);
        } else {
            if (nodes[u].count == 0) swap(u, v);
            int w = nodes[u].L.begin()->first;
            _move_out(u);
            nodes[u]._insert_L(v);
            nodes[v]._insert_L(u);
            _move_in(w);
        }
        for (int w : nodes[u].edges)
            if (nodes[w].status == 2 && nodes[w].count == 1)
                _insert_candidate(nodes[w].L.begin()->first, w);
        if (nodes[u].count == 1) _insert_candidate(v, u);
        if (!candidates.empty()) _extend_oneswap();
    } else if (nodes[u].status != nodes[v].status) {
        if (nodes[v].status == 1) swap(u, v);
        if (nodes[v].count == 1) nodes[nodes[v].L.begin()->first]._delete_L(v);
        nodes[v]._insert_L(u);
    }
#ifdef _LINUX_
    gettimeofday(&end, NULL);
    long long mtime, seconds, useconds;
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = seconds*1000000 + useconds;
    utime += mtime;
#endif
}

void Graph::_delete_edge(int u, int v) {
#ifdef _LINUX_
    struct timeval start, end;
    gettimeofday(&start, NULL);
#endif
    m --;
    nodes[u]._delete_edge(v);
    nodes[v]._delete_edge(u);
    if (nodes[u].status == 2 && nodes[v].status == 2) {
        if (nodes[u].count == 1 && nodes[v].count == 1) {
            if (nodes[u].L.begin()->first == nodes[v].L.begin()->first) {
                int w = nodes[u].L.begin()->first;
                _move_out(w);
                _move_in(u);
                _move_in(v);
                for (int id : nodes[w].edges)
                    if (nodes[id].status == 2 && nodes[id].count == 1)
                        _insert_candidate(nodes[id].L.begin()->first, id);
                if (!candidates.empty()) _extend_oneswap();
            }
        }
    } else if (min(nodes[u].status, nodes[v].status) == 1) {
        if (nodes[v].status == 1) swap(u, v);
        if (nodes[v].count == 1) nodes[u]._delete_L(v);
        nodes[v]._delete_L(u);
        if (nodes[v].count == 0) _move_in(v);
        else if (nodes[v].count == 1) {
            nodes[nodes[v].L.begin()->first]._insert_L(v);
            _insert_candidate(nodes[v].L.begin()->first, v);
            _extend_oneswap();
        }
    }
#ifdef _LINUX_
    gettimeofday(&end, NULL);
    long long mtime, seconds, useconds;
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = seconds*1000000 + useconds;
    utime += mtime;
#endif  
}
    
void Graph::_move_in(const int& id) {
    mis ++;
    nodes[id].status = 1;
    nodes[id].L.clear();
    nodes[id].count = 0;
    for (int w : nodes[id].edges) {
        if (nodes[w].status == 2) {
            if (nodes[w].count == 0) nodes[id]._insert_L(w);
            else if (nodes[w].count == 1)
                nodes[nodes[w].L.begin()->first]._delete_L(w);
            nodes[w]._insert_L(id);
        }   
    }
}

void Graph::_move_out(const int& id) {
    mis --;
    nodes[id].status = 2;
    nodes[id].L.clear();
    nodes[id].count = 0;
    for (int w : nodes[id].edges) {
        if (nodes[w].status == 2) {
            nodes[w]._delete_L(id);
            if (nodes[w].count == 1)
                nodes[nodes[w].L.begin()->first]._insert_L(w);
        }
    }
}

void Graph::_extend_oneswap() {
    while (!candidates.empty()) {
        int v = candidates.begin()->first;
        vector<int> newL = candidates.begin()->second;
        candidates.erase(v);
        while (!newL.empty()) {
            int u = newL.back();
            newL.pop_back();
            if (nodes[u].status == 2 && nodes[u].count == 1 && nodes[u].L.begin()->first == v) {
                int intersection = 0;
                for (int w : nodes[u].edges)
                    if (nodes[v].L.count(w) != 0) intersection ++;
                if (intersection + 1 < nodes[v].count) {
                    _move_out(v);
                    _move_in(u);
                    for (int w : nodes[v].edges)
                        if (nodes[w].count == 0 && nodes[w].status == 2) _move_in(w);
                    for (int w : nodes[v].edges)
                        if (nodes[w].count == 1 && nodes[w].status == 2)
                            _insert_candidate(nodes[w].L.begin()->first, w);
                    break;
                }
            }
        }
#ifdef _PERTURBATION_
        if (nodes[v].status == 1 && nodes[v].count > 0) {
            int u = v, min_degree = nodes[v].edges.size();
            for (int w : nodes[v].edges) {
                if (nodes[w].count == 1 && nodes[w].edges.size() < min_degree) {
                    u = w;
                    min_degree = nodes[w].edges.size();
                }
            }
            if (u != v) {
                _move_out(v);
                _move_in(u);
                for (int w : nodes[v].edges)
                    if (nodes[w].count == 0 && nodes[w].status == 2) _move_in(w);
                for (int w : nodes[v].edges)
                    if (nodes[w].count == 1 && nodes[w].status == 2)
                        _insert_candidate(nodes[w].L.begin()->first, w);
            }
        }
#endif
    }
}

int main(int argc, char const *argv[])
{
    string id = string(argv[1]);
    string graph_path = id + ".graph";
    string mis_path = id + ".mis";
    string inst_path = id + ".inst";
    Graph g;
    g.load_graph(graph_path.c_str());
    g.load_mis(mis_path.c_str());
    g.check();
    g.eff_exp(inst_path.c_str(), atoi(argv[2]));
    g.check();
    return 0;
}
