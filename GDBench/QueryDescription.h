//===-- traversals/QueryDescription.h - Query class ----------*- C++ -*-===//
//
//                     CAESR Graph Database 
//
// TODO: LICENSE
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This is the class for GDBench queries.
///
//===----------------------------------------------------------------------===//

#ifndef _QUERYDESCRIPTION_H_
#define _QUERYDESCRIPTION_H_

#include "BreadthFirstSearch.h"
#include "DepthFirstSearch.h"
#include "CustomVisitor.h"
#include "RecursiveDepthFirstSearch.h"

#define _PRINTGDB_ 1

#ifdef _PRINTGDB_
#include <fstream>
std::ofstream GDFile;
#endif

class Query {
public:
  typedef std::string KeyType;
  typedef std::string ValueType;
  typedef GraphType Graph;
  /// TraversalMethodSwitch: 1: bfs 2: dfs
  typedef unsigned int  TraversalType; 
  typedef GraphType::VertexDescriptor VertexDescriptor;
public:
  Query() { }
  
  virtual void runQuery(Graph & graph, TraversalType Traversal) { }

  void setPersonProperty(const KeyType & key, const ValueType & value) {
    Key = key;
    Value = value;
  }

  void setPersonId (VertexDescriptor Id) {
    PersonId = Id; 
  }

  void setEndPoints(const VertexDescriptor & Id1, const VertexDescriptor & Id2) {
    PersonId1 = Id1;
    PersonId2 = Id2;
  }

  void setWebId(VertexDescriptor webId) {
    WebId = webId;
  }
protected:
  KeyType   Key;
  ValueType Value;
  VertexDescriptor WebId;
  VertexDescriptor PersonId;
  VertexDescriptor PersonId1;
  VertexDescriptor PersonId2;
};

class Query14 : public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal) {
    Visitor v1;
    switch(Traversal) {
      case 1:
        filtProperty(Key, Value, v1.getFilter());
        breadthFirstSearch(graph, 0, v1 );
        break;
      case 2:
        filtProperty(Key, Value, v1.getFilter());
        depthFirstSearch(graph, 0, v1);
        break;
    }

#ifdef _PRINTGDB_
  GDFile.open("gd_execution.log", std::ios_base::out | std::ios_base::app);
  GDFile << "Query 14\n";
  if(Traversal == 1)
      GDFile << "---------------------BFS---------------------\n";
  else
      GDFile << "---------------------DFS---------------------\n";
  GDFile << "Traversal from vertex 0.\n";
  GDFile.close();
#endif
 }

};


class Query1 : public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal) {
    SelectionVisitor v1;
    filtProperty(Key, Value, v1.getFilter());
    traverseThroughType("FRIENDS", v1.getFilter());
    switch(Traversal) {
      case 1:
        breadthFirstSearch(graph, 0, v1 );
        break;
      case 2:
        depthFirstSearch(graph, 0, v1);
        break;
      case 3:
        recursiveDepthFirstSearch(graph, 0, v1);
    }

#ifdef _PRINTGDB_
    GDFile.open("gd_execution.log", std::ios_base::out | std::ios_base::app);
    GDFile << "Query 1\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else if (Traversal == 2)
        GDFile << "---------------------DFS---------------------\n";
    else 
        GDFile << "----------------recursiveDFS-----------------\n";
    GDFile <<"People with " << Key << " = " 
           << Value <<" is(are) as below\n";
    auto target = v1.getVertexList();
    for(auto it = target.begin(); it != target.end(); ++it) {
      GDFile << "Vertex " << (*it)->getId();
      GDFile << "\n";
    }
    GDFile << "\n";;
    GDFile.close();
#endif
 }

};


class Query2 : public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal) {
    AdjacencyVisitor v2; 
    traverseThroughTypeAndDirection("LIKES","in", v2.getFilter());
    switch(Traversal) {
      case 1:
        breadthFirstSearch(graph, WebId, v2);
        break;
      case 2:
        depthFirstSearch(graph, WebId, v2);
        break;
      case 3:
        recursiveDepthFirstSearch(graph,WebId, v2);
        break;
    }

#ifdef _PRINTGDB_
    GDFile.open("gd_execution.log", std::ios_base::out | std::ios_base::app);
    GDFile << "Query 2\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else if (Traversal == 2)
        GDFile << "---------------------DFS---------------------\n";
    else 
        GDFile << "----------------recursiveDFS-----------------\n";

    GDFile << "People who likes webpage id = " 
           << WebId  << " are as below\n";
    auto target= v2.getVertexList();
    for(auto it = target.begin(); it != target.end(); ++it) {
      std::string key("name");
      GDFile << "Vertex " << (*it)->getId() <<"\t" 
             << (*it)->getPropertyValue(key).first;
      GDFile << "\n";
    }
    GDFile << "\n";
    GDFile.close();
#endif
 }
};


class Query3 : public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal ) {
    AdjacencyVisitor v3; 
    traverseThroughType("LIKES", v3.getFilter());
    switch(Traversal) {
      case 1:
        breadthFirstSearch(graph, PersonId, v3);
        break;
      case 2:
        depthFirstSearch(graph, PersonId, v3);
        break;
      case 3:
        recursiveDepthFirstSearch(graph, PersonId, v3);
        break;
    }

#ifdef _PRINTGDB_
    GDFile.open("gd_execution.log", std::ios_base::out | std::ios_base::app);
    GDFile << "Query 3\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else if (Traversal == 2)
        GDFile << "---------------------DFS---------------------\n";
    else
        GDFile << "----------------recursiveDFS-----------------\n";

    auto target = v3.getVertexList();
    GDFile << "Person with vid = " << PersonId << " likes webpages: " 
           << target.size() <<" in totoal \n";
    for(auto it = target.begin(); it != target.end(); ++it) {
      std::string key("URL");
      GDFile <<"Vertex " << (*it)->getId() << "\t"  
             << (*it)->getPropertyValue(key).first;
      GDFile << "\n";
    }
    GDFile << "\n";
    GDFile.close();
#endif
 }
};

class Query4 : public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal) {
    SelectionVisitor  v4;
    switch(Traversal) {
      case 1:
        filtProperty(Key, Value, v4.getFilter());
        traverseThroughType("FRIENDS", v4.getFilter());
        breadthFirstSearch(graph, 0, v4);
        break;
      case 2:
        filtProperty(Key, Value, v4.getFilter());
        traverseThroughType("FRIENDS", v4.getFilter());
        depthFirstSearch(graph, 0, v4);
        break;
    }

#ifdef _PRINTGDB_
    GDFile << "Query 4\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
    GDFile <<"People with " << Key << " = " << Value <<" is(are) as below\n";
    auto target = v4.getVertexList();
    for(auto it = target.begin(); it != target.end(); ++it) {
      std::string key("name");
      GDFile <<"Vertex " << (*it)->getId() << "\t" 
             << (*it)->getPropertyValue(key).first;
      GDFile << "\n";
    }
#endif
 }
};

class Query5 : public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal ) {

#ifdef _PRINTGDB_
    GDFile << "Query 5\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
    GDFile << "The friends of Person with vid = " << PersonId << " has friends\n";
#endif
    FilterType tmpFilter[2];
    std::string key("name");
    std::vector<VertexPointer> target;
    switch(Traversal) {
      case 1: {
        traverseThroughTypeAndDirection("FRIENDS", "out",  tmpFilter[0]);
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[1]);
        ReachabilityVisitor  v5b;
        v5b.setFilter(tmpFilter[0]);
        v5b.setFilter(tmpFilter[1]);
        v5b.setDepth(2);
        breadthFirstSearch(graph, PersonId, v5b);
#ifdef _PRINTGDB_
        auto TargetSet = v5b.getTargetSet(); 
        for(auto it = TargetSet.begin(); it != TargetSet.end(); ++it) {
          GDFile <<"Vertex " << (*it)->getId() << "\t" 
                 << (*it)->getPropertyValue(key).first << "\n";
        }
#endif
        break;
              }
      case 2: {
        traverseThroughTypeAndDirection("FRIENDS", "out",  tmpFilter[0]);
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[1]);
        DFSReachabilityVisitor v5d;
        v5d.setFilter(tmpFilter[0]);
        v5d.setFilter(tmpFilter[1]);
        v5d.setDepth(2);
        depthFirstSearch(graph, PersonId, v5d);
        auto target = v5d.getVertexTargetMap();

#ifdef _PRINTGDB_
        GDFile << "---------------------DFS---------------------\n";
        for(auto it = target.begin(); it != target.end(); ++it) {
          GDFile <<"Vertex " << (*it).first->getId() << "\t" 
                 << (*it).first->getPropertyValue(key).first;
        GDFile << "\n";
        }
#endif
        break;
              }
    }

  }
};

class Query6: public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal) {

#ifdef _PRINTGDB_
    GDFile << "Query 6\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
#endif
    std::string key("wpurl");
    FilterType tmpFilter[2];
    switch(Traversal) {
      case 1: {
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[0]);
        traverseThroughTypeAndDirection("LIKES", "out", tmpFilter[1]);
        ReachabilityVisitor  v6b;
        v6b.setFilter(tmpFilter[0]);
        v6b.setFilter(tmpFilter[1]);
        v6b.setDepth(2);
        breadthFirstSearch(graph, PersonId, v6b);

#ifdef _PRINTGDB_
        auto TargetSet = v6b.getTargetSet(); 
        GDFile << "The friends of Person with vid = " << PersonId 
               << " like " << TargetSet.size() << " webpages\n";
        for(auto it = TargetSet.begin(); it != TargetSet.end(); ++it) {
          GDFile <<"Vertex " << (*it)->getId() << "\t" 
                 << (*it)->getPropertyValue(key).first << "\n";
        }
#endif
        break;
              }
      case 2: {
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[0]);
        traverseThroughTypeAndDirection("LIKES", "out", tmpFilter[1]);
        DFSReachabilityVisitor v6d;
        v6d.setFilter(tmpFilter[0]);
        v6d.setFilter(tmpFilter[1]);
        v6d.setDepth(2);
        depthFirstSearch(graph, PersonId, v6d);

#ifdef _PRINTGDB_
        auto target = v6d.getVertexTargetMap();
        GDFile << "The friends of Person with vid = " << PersonId 
               << " like " << target.size() << " webpages\n";
        for(auto it = target.begin(); it != target.end(); ++it) {
          GDFile <<"Vertex " << (*it).first->getId() << "\t" 
                 << (*it).first->getPropertyValue(key).first;
        GDFile << "\n";
        }
#endif
        break;
              }
    }
  }
};

class Query7: public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal ) {

#ifdef _PRINTGDB_
    GDFile << "Query 7\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
#endif
    FilterType tmpFilter[2];
    std::string key("name");
    switch(Traversal) {
      case 1: {
        traverseThroughType("LIKES", tmpFilter[0]);
        traverseThroughType("LIKES", tmpFilter[1]);
        ReachabilityVisitor  v7b;
        v7b.setFilter(tmpFilter[0]);
        v7b.setFilter(tmpFilter[1]);
        v7b.setDepth(2);
        breadthFirstSearch(graph, PersonId, v7b);

#ifdef _PRINTGDB_
        auto TargetSet = v7b.getTargetSet(); 
        GDFile << "The webpages liked by person vid = " << PersonId 
               << " are liked by " << TargetSet.size() << " people: \n";
        for(auto it = TargetSet.begin(); it != TargetSet.end(); ++it) {
          GDFile <<"Vertex " << (*it)->getId() << "\t" 
                 << (*it)->getPropertyValue(key).first << "\n";
        }
#endif
        break;
              }
      case 2: {
        traverseThroughType("LIKES", tmpFilter[0]);
        traverseThroughType("LIKES", tmpFilter[1]);
        DFSReachabilityVisitor v7d;
        v7d.setFilter(tmpFilter[0]);
        v7d.setFilter(tmpFilter[1]);
        v7d.setDepth(2);
        depthFirstSearch(graph, PersonId, v7d);

#ifdef _PRINTGDB_
        auto target = v7d.getVertexTargetMap();
        GDFile << "The webpages liked by person vid = " << PersonId 
               << " are liked by  " << target.size() << " people\n";
        for(auto it = target.begin(); it != target.end(); ++it) {
          GDFile <<"Vertex " << (*it).first->getId() << "\t" 
                 << (*it).first->getPropertyValue(key).first;
          GDFile << "\n";
        }
#endif
        break;
              }
    }

  }
};

class Query8 : public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal) {

#ifdef _PRINTGDB_
    GDFile << "Query 8\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
#endif
    switch(Traversal) {
      case 1: {
        PathVisitor v8b;
        v8b.setEndVertex(PersonId2);
        breadthFirstSearch(graph, PersonId1, v8b);
#ifdef _PRINTGDB_
        auto target = v8b.getVertexList();
        if(!target.empty())
          GDFile << "There is path from " << PersonId1 << " to " <<  PersonId2 << "\n";
        else 
          GDFile << PersonId1 << " and " <<  PersonId2 <<" are not connected" << "\n";
#endif
        break;
              }
      case 2: {
        DFSPathVisitor v8d;
        v8d.setEndVertex(PersonId2);
        depthFirstSearch(graph, PersonId1, v8d);
#ifdef _PRINTGDB_
        auto target = v8d.getPathList();
        if(!target.empty())
          GDFile << "There is path from " << PersonId1 << " to " <<  PersonId2 << "\n";
        else 
          GDFile << PersonId1 << " and " <<  PersonId2 <<" are not connected" << "\n";
#endif
        break;
              }
    }
  }
};

class Query9: public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal ) {
#ifdef _PRINTGDB_
    GDFile << "Query 9\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
#endif
    //PathVisitor v9;
    //v9.setEndVertex(PersonId2);
    switch(Traversal) {
      case 1: {
        PathVisitor v9b;
        v9b.setEndVertex(PersonId2);
        breadthFirstSearch(graph, PersonId1, v9b);
#ifdef _PRINTGDB_
        auto target = v9b.getVertexList();
        if(target.empty())
          GDFile << PersonId1 << " and " <<  PersonId2 <<" are not connected" << "\n";
        else {
            GDFile << "There are  shortest paths from " << PersonId1 
                   << " to " <<  PersonId2 << "\n";
          for(auto it = target.begin(); it != target.end(); ++it) {
            GDFile <<"Vertex " << (*it)->getId() << "\n";
          }
        }
#endif
        break;
              }
      case 2: {
        DFSShortestPathVisitor v9d;
        v9d.setEndVertex(PersonId2);
        depthFirstSearch(graph, PersonId1, v9d);
#ifdef _PRINTGDB_
        auto target = v9d.getPathList();
        if(target.empty())
          GDFile << PersonId1 << " and " <<  PersonId2 <<" are not connected" << "\n";
        else {
          GDFile << "There are " << target.size() << " shortest paths from " 
                 << PersonId1 << " to " <<  PersonId2 << "\n";
          for(auto it = target.begin(); it != target.end(); ++it) {
            for (auto iter = (*it).second.begin(); iter != (*it).second.end(); ++iter)
              GDFile <<"Vertex " << (*iter)->getId() << "\n";
          GDFile << "\n";
         }
       }
#endif
        break;
              }
    }
  }
};

class Query10 : public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal ) { 
    FilterType tmpFilter[2];
    std::string key("name");
    std::vector<VertexPointer> target;
    switch(Traversal) {
      case 1: {
        traverseThroughTypeAndDirection("FRIENDS", "out",  tmpFilter[0]);
        traverseThroughTypeAndDirection("FRIENDS", "in", tmpFilter[1]);
        PatternVisitor v10d;
        v10d.setFilter(tmpFilter[0]);
        v10d.setFilter(tmpFilter[1]);
        v10d.setDepth(2);
        v10d.setEndVertex(PersonId2);
        breadthFirstSearch(graph, PersonId1, v10d);
        target = v10d.getVertexList();
        break;
              }
      case 2: {
        traverseThroughTypeAndDirection("FRIENDS", "out",  tmpFilter[0]);
        traverseThroughTypeAndDirection("FRIENDS", "in", tmpFilter[1]);
        DFSPatternVisitor v10d;
        v10d.setFilter(tmpFilter[0]);
        v10d.setFilter(tmpFilter[1]);
        v10d.setDepth(2);
        v10d.setEndVertex(PersonId2);
        depthFirstSearch(graph, PersonId1, v10d);
        target = v10d.getVertexList();
        break;
              }
    }
#ifdef _PRINTGDB_
    GDFile << "Query 10\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
    GDFile << "There are " << target.size() << " common friends between  " 
           << PersonId1 << " and " <<  PersonId2 << "\n";
    for(auto it = target.begin(); it != target.end(); ++it) {
      GDFile << "Vertex " << (*it)->getId() << "\t" 
             << (*it)->getPropertyValue(key).first;
    GDFile << "\n";
    }
#endif
  }
};


class Query11: public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal) {
    FilterType tmpFilter[2];
    std::string key("name");
    std::vector<VertexPointer> target;
    switch(Traversal) {
      case 1: {
        traverseThroughTypeAndDirection("LIKES", "out",  tmpFilter[0]);
        traverseThroughTypeAndDirection("LIKES", "in",   tmpFilter[1]);
        PatternVisitor v11b;
        v11b.setFilter(tmpFilter[0]);
        v11b.setFilter(tmpFilter[1]);
        v11b.setDepth(2);
        v11b.setEndVertex(PersonId2);
        breadthFirstSearch(graph, PersonId1, v11b);
        target = v11b.getVertexList();
        break;
              }
      case 2: {
        traverseThroughTypeAndDirection("LIKES", "out",  tmpFilter[0]);
        traverseThroughTypeAndDirection("LIKES", "in",   tmpFilter[1]);
        DFSPatternVisitor v11d;
        v11d.setFilter(tmpFilter[0]);
        v11d.setFilter(tmpFilter[1]);
        v11d.setDepth(2);
        v11d.setEndVertex(PersonId2);
        depthFirstSearch(graph, PersonId1, v11d);
        target = v11d.getVertexList();
        break;
              }
    }
#ifdef _PRINTGDB_
    GDFile << "Query 11\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
    GDFile << "There are " << target.size() 
           << " common webpages liked by both " 
           << PersonId1 << " and " <<  PersonId2 << "\n";
    for(auto it = target.begin(); it != target.end(); ++it) {
      std::string key("wpurl");
      GDFile << "Vertex " << (*it)->getId() << "\t" 
             << (*it)->getPropertyValue(key).first;
      GDFile << "\n";
    }
#endif

  }
};

class Query12 : public Query {
public:
 virtual void runQuery(Graph & graph, TraversalType Traversal ) {
   std::vector<VertexPointer>  target;
    switch(Traversal) {
      case 1: {
        AdjacencyVisitor v12b; 
        traverseThroughTypeAndDirection("FRIENDS", "out", v12b.getFilter());
        breadthFirstSearch(graph, PersonId, v12b);
        target = v12b.getVertexList();
        break;
              }
      case 2: {
        AdjacencyVisitor v12d; 
        traverseThroughTypeAndDirection("FRIENDS", "out",  v12d.getFilter());
        depthFirstSearch(graph, PersonId, v12d);
        target = v12d.getVertexList();
        break;
              }
    }
#ifdef _PRINTGDB_
    GDFile << "Query 12\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
    std::string key("name");
    GDFile << "Person with vid = " << PersonId << " has name: " 
           << graph.getVertexPointer(PersonId)->getPropertyValue(key).first 
           <<" and  " << target.size() << " friends\n";
    for (auto it = target.begin(); it != target.end(); it++)
      GDFile << "Vertex " << (*it)->getId() << "\n";
#endif
 }
};

class Query13: public Query {
public:
  virtual void runQuery(Graph & graph, TraversalType Traversal ) {
#ifdef _PRINTGDB_
    GDFile << "Query 13\n";
    if(Traversal == 1)
        GDFile << "---------------------BFS---------------------\n";
    else
        GDFile << "---------------------DFS---------------------\n";
#endif
    FilterType tmpFilter[3];
    std::string key("name");
    switch(Traversal) {
      case 1: {
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[0]);
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[1]);
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[2]);
        ReachabilityVisitor  v13b;
        v13b.setFilter(tmpFilter[0]);
        v13b.setFilter(tmpFilter[1]);
        v13b.setFilter(tmpFilter[2]);
        v13b.setDepth(3);
        breadthFirstSearch(graph, PersonId, v13b);
#ifdef _PRINTGDB_
        auto TargetSet = v13b.getTargetSet(); 
        GDFile << "The friends of friends of person vid = " << PersonId 
               << " has " << TargetSet.size() << " friends: \n";
        for(auto it = TargetSet.begin(); it != TargetSet.end(); ++it) {
          GDFile <<"Vertex " << (*it)->getId() << "\t" 
                << (*it)->getPropertyValue(key).first << "\n";
        }
#endif
        break;
              }
      case 2: {
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[0]);
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[1]);
        traverseThroughTypeAndDirection("FRIENDS", "out", tmpFilter[2]);
        DFSReachabilityVisitor v13d;
        v13d.setFilter(tmpFilter[0]);
        v13d.setFilter(tmpFilter[1]);
        v13d.setFilter(tmpFilter[2]);
        v13d.setDepth(3);
        depthFirstSearch(graph, PersonId, v13d);
#ifdef _PRINTGDB_
        auto target = v13d.getVertexTargetMap();
        GDFile << "The friends of friends of person vid = " << PersonId 
               << " has " << target.size() << " friends: \n";
        for(auto it = target.begin(); it != target.end(); ++it) {
        if( (*it).first->getId() != PersonId ) {
        GDFile << "Vertex " << (*it).first->getId() << "\t"  
               << (*it).first->getPropertyValue(key).first;
        GDFile << "\n";
        }
      }
#endif
        break;
              }
    }
  }
};

#endif /**_QUERYDESCRIPTION_H_ */
