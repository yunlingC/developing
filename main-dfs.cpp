#include "Vertex.h"
#include "Edge.h"
#include "GraphType.h"
#include "FixedString.h"
#include "macros.h"
#include "GDReader.h"
#include "LinkedList.h"

#include <iostream>
#include <queue>

#include "RecursiveDepthFirstSearch.h"
/// Test
#include "tests/gs/g1.h"

using namespace std;

int main() {

  typedef GraphType Graph;

  Graph g;
  cout << "Begin testing\n";
  GDReader reader(g);
  reader.readFile("../tests/gd/sndata.gd");

  //createGraph(g);
  Graph::VertexPtr vp0 = g.getVertexPointer(0);
  cout << "DFS start\n";
  depthFirstSearch(g, 0);

  return 0;
}