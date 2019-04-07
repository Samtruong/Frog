#include <iostream>
using namespace std;


#include "graph.hpp"

typedef graph_t<int, int> Graph;

int main(int argc, char **argv)
{
    if (argc >2) {
        Graph g;
        cout << "Reading mtx file: '" << argv[1] << "' ..." << endl;
        g.read_mtx_file(argv[1]);
        cout << "Done Reading" << endl << endl;
        cout << "Writing bin file: '" << argv[2] << "' ..." << endl;
        g.write_bin_file(argv[2]);
        cout << "Done Writing" << endl << endl;
        g.release();
    } else {
        cout << "Usage: " << argv[0] << " src_mtx dst_bin" << endl;
    }
    return 0;
}
