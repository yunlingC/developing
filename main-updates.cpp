#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <queue>
#include <thread>
//#include <boost/thread.hpp>
#include "GDBench/util.h"
#include "LDBCReader.h"
#include "LinkedList.h"
//#include "LdbcQueryDescription.h"
#include "LdbcUpdateDescription.h"
#include "QueryRandomizer.h"


int main() {

  typedef GraphType Graph;
  typedef std::thread thread;

  Graph g;
  cout << "Begin testing\n";

//  GDReader reader(g);
//  reader.readFile("../tests/gd/sndata.gd");
//
  LDBCReader reader(g);
  reader.readDirectory("../tests/ldbc/social_network_10/");
 
  LockManager lockManager;
  lockManager.buildLockMap(g);
  cout << "LockMap built\n";

  TransactionManager transManager;

  ///Q16 is for insert: insert a new people make is MOHAMMED 's new friend
  Query16 Q16;
  Q16.setPersonProperty("firstName","Luis Filipe "); 

  //Q18 is for reading people MOHAMMED 's friends number: 7 orginally
  Query18 Q18;
  Q18.setPersonId(0);

  ///without concurrency: results is in gd_excution.log (not logfile for rollback!)
  Q18.runQuery(g, transManager, lockManager, 1);
  Q16.runQuery(g, transManager, lockManager, 1);
  Q18.runQuery(g, transManager, lockManager, 1);

  ///In order for observe how locks is working, run every thread twice;
  // sleep time could be changed in order to see how data are locked and released
  MyFile << "\n\n++++++++++++++Multithread+++++++++++++++\n\n";
  vector<thread> threads;
//  threads.push_back(thread([&] {Q15.runQuery(g, transManager, lockManager, 1);}));
  threads.push_back(thread([&] {Q18.runQuery(g, transManager, lockManager, 1);}));
  threads.push_back(thread([&] {Q16.runQuery(g, transManager, lockManager, 1);}));
  threads.push_back(thread([&] {Q16.runQuery(g, transManager, lockManager, 1);}));
//  threads.push_back(thread([&] {Q17.runQuery(g, transManager, lockManager, 1);}));
//  threads.push_back(thread([&] {Q17.runQuery(g, transManager, lockManager, 1);}));

  for_each(threads.begin(), threads.end(),
           std::mem_fn(&thread::join));

  Q18.runQuery(g, transManager,lockManager,  1);

  cout << "finish testing\n";
  MyFile.close();
  return 0;
}
