//#include "macros.h"
#include "GDReader.h"
#include "LinkedList.h"
#include "breadth_first_search.h"
#include "QueryDescription.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <thread>

int main() {

  typedef GraphType Graph;

  Graph g;
  //  g.allocVertexMemory(50);
  //  g.allocEdgeMemory(100);
  cout << "Begin testing\n";

  GDReader reader(g);
  reader.readFile("../tests/gd/sndata100.gd");

  //createGraph(g);
  //  Graph::VertexPtr vp0 = g.getVertexPointer(0);
  cout << "BFS start\n";

  Query1 Q1;
  Q1.setPersonProperty("name" , "KIRA VERLATO");

  Query2 Q2;
  Query3 Q3;

  Query4 Q4;
  Q4.setPersonProperty("pid", "5");

  Query5 Q5;

  Query6 Q6;

  Query7 Q7;

  Query8 Q8;
  
  Query9 Q9;

  Query10 Q10;

  Query11 Q11;

  Query12 Q12;
  
  Query13 Q13;
   
  Q1.runQuery(g);
  Q2.runQuery(g, 14);
  Q3.runQuery(g, 4);
  Q4.runQuery(g);
  Q5.runQuery(g, 0);
  Q6.runQuery(g, 0);
  Q7.runQuery(g, 0);
  Q8.runQuery(g, 0, 2);
  Q9.runQuery(g, 0, 99);
  Q10.runQuery(g, 0, 1);
  Q11.runQuery(g, 0, 1);
  Q12.runQuery(g, 4);
  Q13.runQuery(g, 0);

/*
  vector<thread> threads;
  threads.push_back( thread([&]{Q1.runQuery(g);}));
  threads.push_back( thread([&]{Q2.runQuery(g, 14);}));
  threads.push_back( thread([&]{Q3.runQuery(g, 4);}));
  
//  threads.push_back( thread( &Query::runQuery3, Q));
//  threads.push_back( thread( &Query::runQuery4, Q));
//  threads.push_back( thread( &Query::runQuery12, Q));

  for_each(threads.begin(), threads.end(),
           std::mem_fn(&thread::join));

*/

/*
  Q.runQuery1();

//  Query Q2;
  Q.runQuery2(); 

//  Query Q3;
  Q.runQuery3();

//  Query Q4;
  Q.runQuery4();

//  Query1 Q12;
  Q.runQuery12();
*/
  return 0;
}
