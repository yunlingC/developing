//===-- LDBC/LdbcQueryDescription.h - Query class ----------*- C++ -*-===//
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

#ifndef _LDBCQUERYDESCRIPTION_H_
#define _LDBCQUERYDESCRIPTION_H_

#include <vector>
#include <string>
#include <iostream>

#include "LdbcCustomVisitor.h"
#include "BreadthFirstSearch.h"
#include "QueryDescription.h"


class LdbcQuery1 : public Query {
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 1================\n";
    Filter tmpFilter[3];
    Filter NameFilter;
    string name = "Benedejcic";
    filtProperty("lastName", name, NameFilter); 
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[0]);
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[1]);
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[2]);
        LimitedDepthVisitor  v1;
        v1.setFilter(tmpFilter[0]);
        v1.setFilter(tmpFilter[1]);
        v1.setFilter(tmpFilter[2]);
        v1.setNameFilter(NameFilter);
        v1.setDepth(3);
        breadthFirstSearch(graph, startVertex, v1);
        auto target = v1.getVertexTargetList();
        cout << startVertex << " is connected with " << target.size() << " people with firstName : " << name << endl;
        for(auto it = target.begin(); it != target.end(); ++it) {
          cout <<"Vertex " << (*it)->getId() << "\t" << (*it)->getPropertyValue("id").first << (*it)->getPropertyValue("lastName").first;
        cout << endl;
    }
  }
};

class LdbcQuery2 : public Query {
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 2================\n";
    Filter tmpFilter[3];
    traverseThroughMultiRelType("KNOWS", tmpFilter[0]); 
    traverseThroughMultiRelType("COMMENT_HAS_CREATOR+POST_HAS_CREATOR", tmpFilter[1]); 
    tmpFilter[2].setValueRange("creationDate", "", "2012-07-16T10:00:00.255"); 
   
        MultiRelTypeVisitor v2;
        v2.setFilter(tmpFilter[0]);
        v2.setFilter(tmpFilter[1]);
        v2.setRangeFilter(tmpFilter[2]);
        v2.setDepth(2);
        v2.setDepthToCheckRange(2);
        v2.setPropToCheck(2); //check date
        breadthFirstSearch(graph, startVertex, v2);
        auto target = v2.getVertexTargetList();
        auto targets = v2.getTargetsMap();
        cout << startVertex << " has friends made " << target.size() << " comments and posts \n";
        for(auto it = targets.begin(); it != targets.end(); ++it) {
          cout <<"person " << (*it).second->getPropertyValue("id").first << "\t" <<"comments/posts " << (*it).first->getPropertyValue("id").first << "\t" << (*it).first->getPropertyValue("creationDate").first ;
          cout << endl;
        }
  }

};


class LdbcQuery3 : public Query {
public:
  typedef map<VertexPointer, vector<VertexPointer>> PersonListMapType;
  typedef pair<VertexPointer, vector<VertexPointer>> PersonListPair;
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 3================\n";
    Filter tmpFilter[2];
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[0]);
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[1]);

    SingleRelTypeVisitor v3;
    v3.setFilter(tmpFilter[0]);
    v3.setFilter(tmpFilter[1]);
    v3.setDepth(2);
    breadthFirstSearch(graph, startVertex, v3);
    auto target = v3.getVertexTargetList();
    cout << startVertex << " is connected with " << target.size() << " friends and friends of friends" << endl;
    Filter FilterSet[4];
    traverseThroughMultiRelType("PERSON_IS_LOCATED_IN", FilterSet[0]); 
    traverseThroughMultiRelType("IS_PART_OF", FilterSet[1]); 
    auto country1 = "Austria";
    auto country2 = "Angola";
    FilterSet[2].setProperty("name", country1);
    FilterSet[3].setProperty("name", country2);
    
    auto it = target.begin(); 
    while(it != target.end()) {
      VertexPropertyRangeVisitor vp3;
      vp3.setFilter(FilterSet[0]);
      vp3.setFilter(FilterSet[1]);
      vp3.setVertexFilters(FilterSet[2]);
      vp3.setVertexFilters(FilterSet[3]);
      vp3.setDepth(2);
      vp3.setDepthToCheckVertexProp(2);
      auto startVertex = (*it)->getId();
      breadthFirstSearch(graph, startVertex, vp3);
      if(vp3.getIncludeState() == false) {
        target.erase(it);
      }
      else {
        it++;
      }
    }

    Filter Filters[5];
    PersonListMapType PersonListMap;
    traverseThroughMultiRelType("COMMENT_HAS_CREATOR+POST_HAS_CREATOR",Filters[0]); 
    Filters[1].setValueRange("creationDate", "2010-01-00T00:01:00.255", "2013-07-16T23:59:00.255"); 
    traverseThroughMultiRelType("POST_IS_LOCATED_IN+COMMENT_IS_LOCATED_IN",Filters[2]); 
    Filters[3].setProperty("name", country1); 
    Filters[4].setProperty("name", country2); 
    
    for (auto it = target.begin(); it != target.end(); it++) {
      VertexPropertyRangeVisitor vpr3;
      vpr3.setFilter(Filters[0]);
      vpr3.setFilter(Filters[2]);
      vpr3.setRangeFilter(Filters[1]);
      vpr3.setVertexFilters(Filters[4]);
      vpr3.setVertexFilters(Filters[3]);
      vpr3.setDepth(2);
      vpr3.setDepthToCheckRange(2);
      vpr3.setPropToCheck(2);
      vpr3.setDepthToCheckVertexProp(2);
      auto startVertex = (*it)->getId();
      breadthFirstSearch(graph, startVertex, vpr3);
      auto targetList = vpr3.getVertexTargetList();
      PersonListMap.insert(PersonListPair((*it), targetList));
    }

    for(auto it = PersonListMap.begin(); it != PersonListMap.end(); ++it) {
      cout <<"friend " << (*it).first->getPropertyValue("firstName").first  << " has " <<(*it).second.size() << " commmets/posts made in " << country1 << " or " << country2 << endl; 
    }
  }
};

class LdbcQuery4 : public Query {
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 4================\n";
    Filter tmpFilter[4];
    traverseThroughMultiRelType("KNOWS", tmpFilter[0]); 
    traverseThroughMultiRelType("POST_HAS_CREATOR", tmpFilter[1]); 
    traverseThroughMultiRelType("POST_HAS_TAG", tmpFilter[2]); 
    tmpFilter[3].setValueRange("creationDate", "2010-01-00T00:01:00.255", "2013-07-16T23:59:00.255"); 
   
    MultiResultVisitor v4;
    v4.setFilter(tmpFilter[0]);
    v4.setFilter(tmpFilter[1]);
    v4.setFilter(tmpFilter[2]);
    v4.setRangeFilter(tmpFilter[3]);
    v4.setDepth(3);
    v4.setDepthToCheckRange(2);
    v4.setPropToCheck(1); //check time
    breadthFirstSearch(graph, startVertex, v4);
    auto targets = v4.getReturnResultMap();
    cout << startVertex << " has friends made posts of " << targets.size() << " tags\n";
    for(auto it = targets.begin(); it != targets.end(); ++it) {
      cout <<"tags " << (*it).first->getPropertyValue("id").first << "\t" <<"num of posts " << (*it).second <<  endl;
    }
  }
};

class LdbcQuery5 : public Query {
public:
  typedef map<VertexPointer, vector<VertexPointer>> PersonListMapType;
  typedef pair<VertexPointer, vector<VertexPointer>> PersonListPair;
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 5================\n";
    Filter tmpFilter[2];
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[0]);
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[1]);

    SingleRelTypeVisitor v5;
    v5.setFilter(tmpFilter[0]);
    v5.setFilter(tmpFilter[1]);
    v5.setDepth(2);
    breadthFirstSearch(graph, startVertex, v5);
    auto target = v5.getVertexTargetList();
    cout << startVertex << " is connected with " << target.size() << " friends and friends of friends" << endl;
    Filter Filters[4];
    traverseThroughMultiRelType("HAS_MEMBER", Filters[0]); 
    Filters[1].setValueRange("joinDate", "2012-01-00T00:01:00.255", "2013-07-16T23:59:00.255"); 
    traverseThroughMultiRelType("CONTAINER_OF",Filters[2]); 
    traverseThroughMultiRelType("POST_HAS_CREATOR",Filters[3]); 
    vector<Filter> VertexFilter;
    for(auto iter = target.begin(); iter != target.end(); iter++) {
      Filter newFilter;
      newFilter.setProperty("id", (*iter)->getPropertyValue("id").first.std_str());
      VertexFilter.push_back(newFilter);
    }

    map<VertexPointer, unsigned int> targetMap;
    for(auto it = target.begin(); it != target.end(); it++) {
      MultiPropertyVisitor mpv5;
      mpv5.setFilter(Filters[0]);
      mpv5.setFilter(Filters[2]);
      mpv5.setFilter(Filters[3]);
      mpv5.setRangeFilter(Filters[1]);
      mpv5.setVertexFilterList(VertexFilter);
      mpv5.setDepth(3);
      mpv5.setDepthToCheckVertexProp(3);
      mpv5.setDepthToCheckRange(1);
      mpv5.setPropToCheck(2); //check date 
      auto startVertex = (*it)->getId();
      breadthFirstSearch(graph, startVertex, mpv5);
      targetMap.insert(mpv5.getResultMap().begin(), mpv5.getResultMap().end());
    }
    for(auto it = targetMap.begin(); it != targetMap.end(); ++it) {
      if((*it).second != 0)
        cout <<"forum " << (*it).first->getPropertyValue("id").first  << " has " <<(*it).second << " posts made by friends"<< endl; 
    }
  }
};

class LdbcQuery6 : public Query {
public:
  typedef map<VertexPointer, vector<VertexPointer>> PersonListMapType;
  typedef pair<VertexPointer, vector<VertexPointer>> PersonListPair;
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 5================\n";
    Filter tmpFilter[2];
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[0]);
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[1]);

    SingleRelTypeVisitor v6;
    v6.setFilter(tmpFilter[0]);
    v6.setFilter(tmpFilter[1]);
    v6.setDepth(2);
    breadthFirstSearch(graph, startVertex, v6);
    auto target = v6.getVertexTargetList();
    cout << startVertex << " is connected with " << target.size() << " friends and friends of friends" << endl;

    Filter Filters[3];
    traverseThroughMultiRelType("POST_HAS_CREATOR", Filters[0]); 
    traverseThroughMultiRelType("POST_HAS_TAG", Filters[1]); 
    string id = "62";
    Filters[2].setProperty("id", id);
    
    map<string, unsigned int> TagMap; 
    for(auto it = target.begin(); it != target.end(); it++) {
      SinglePropertyVisitor  vpv6;
      vpv6.setFilter(Filters[0]);
      vpv6.setFilter(Filters[1]);
      vpv6.setVertexFilter(Filters[2]);
      vpv6.setDepthToCheckVertexProp(2);
      vpv6.setDepth(2);
      unsigned int startVertex = (*it)->getId();
      breadthFirstSearch(graph, startVertex, vpv6);
      auto personMap = vpv6.getPersonMap(); 
      for(auto it = personMap.begin(); it != personMap.end(); it++) {
        if((*it).second ) {
          for(auto iter = vpv6.getResultTargetsMap()[(*it).first].begin(); 
              iter != vpv6.getResultTargetsMap()[(*it).first].end(); iter++) {
            // (*iter) --> Tag id (string)
            if(TagMap.find(*iter) == TagMap.end()) {
              TagMap.insert(pair<string, unsigned int>((*iter), 1));
            } else {
              TagMap[*iter]++;
              cout << (*iter) << " with " << TagMap[*iter] << " posts \n";
            }
          }
        }
      }
    }
    for (auto it = TagMap.begin(); it != TagMap.end(); it++)
      cout << "Tag " << (*it).first << " has " << (*it).second << " posts\n";
  }
};

class LdbcQuery7 : public Query {
public:
  typedef map<VertexPointer, vector<VertexPointer>> PersonListMapType;
  typedef pair<VertexPointer, vector<VertexPointer>> PersonListPair;
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 5================\n";
    Filter tmpFilter[2];
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[0]);

    SingleRelTypeVisitor v7;
    v7.setFilter(tmpFilter[0]);
    v7.setDepth(1);
    breadthFirstSearch(graph, startVertex, v7);
    auto target = v7.getVertexTargetList();
    vector<Filter> VertexFilter;
    for(auto iter = target.begin(); iter != target.end(); iter++) {
      Filter newFilter;
      newFilter.setProperty("id", (*iter)->getPropertyValue("id").first.std_str());
      VertexFilter.push_back(newFilter);
    }

    cout << startVertex << " is connected with " << target.size() << " friends" << endl;
    Filter Filters[2];
    traverseThroughMultiRelType("COMMENT_HAS_CREATOR+POST_HAS_CREATOR", Filters[0]); 
    traverseThroughMultiRelType("LIKES_COMMENT+LIKES_POST", Filters[1]); 
    VertexMatchVisitor vmv7;
    vmv7.setFilter(Filters[0]);
    vmv7.setFilter(Filters[1]);
    vmv7.setDepth(2);
    vmv7.setVertexFilterList(VertexFilter);
    vmv7.setDepthToCheckVertexProp(2);
    vmv7.setDepthToCompareTime(2);
    breadthFirstSearch(graph, startVertex, vmv7);
    auto targetsMap = vmv7.getTimeMap();
    for(auto it = targetsMap.begin(); it != targetsMap.end(); it++) {
      cout << (*it).first->getPropertyValue("firstName").first << "\t" << (*it).first->getPropertyValue("id").first<< " likes comment/posts at " << (*it).second << endl; 
    }
    auto personMap = vmv7.getPersonMap();
    for(auto it = personMap.begin(); it != personMap.end(); it++)  {
      if((*it).second == true) 
        cout << "comment/post " << (*it).first->getPropertyValue("id").first << " are made by friend\n" ;
    }
  }
};

class LdbcQuery8 : public Query {
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex) {
    cout << "===============Query 8================\n";
    Filter Filters[3];
    traverseThroughMultiRelType("COMMENT_HAS_CREATOR+POST_HAS_CREATOR", Filters[0]); 
    traverseThroughMultiRelType("REPLY_OF_COMMENT+REPLY_OF_POST", Filters[1]); 
    traverseThroughMultiRelType("COMMENT_HAS_CREATOR", Filters[2]); 

    TimeCompareVisitor v8;
    v8.setFilter(Filters[0]);
    v8.setFilter(Filters[1]);
    v8.setFilter(Filters[2]);
    v8.setDepth(3);
    v8.setDepthToCompareTime(2);
    breadthFirstSearch(graph, startVertex, v8);
    auto vertexMap = v8.getVertexMap();
    for(auto it = vertexMap.begin(); it != vertexMap.end(); it++) {
      cout << "Person " << (*it).second->getPropertyValue("firstName").first << " made replies " << (*it).first->getPropertyValue("id").first << " at " << (*it).first->getPropertyValue("creationDate").first << " to startperson's posts/comments\n";
    }
  }
};

class LdbcQuery9 : public Query {
public:
  typedef map<VertexPointer, VertexPointer> ReturnMapType;
  typedef pair<VertexPointer, VertexPointer> ReturnMapPair;
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 9================\n";
    Filter tmpFilter[2];
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[0]);
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[1]);
    SingleRelTypeVisitor v9;
    v9.setFilter(tmpFilter[0]);
    v9.setFilter(tmpFilter[1]);
    v9.setDepth(2);
    breadthFirstSearch(graph, startVertex, v9);
    auto target = v9.getVertexTargetList();
    cout << startVertex << " is connected with " << target.size() << " friends and friends of friends" << endl;
    Filter Filters[2];
    traverseThroughMultiRelType("COMMENT_HAS_CREATOR+POST_HAS_CREATOR",Filters[0]); 
    Filters[1].setValueRange("creationDate", "", "2011-07-16T23:59:00.255"); 
          
    ReturnMapType TargetsMap; 
    for(auto it = target.begin(); it != target.end(); ++it) {
      cout <<"friend " << (*it)->getId() << "\t" << (*it)->getPropertyValue("id").first << "\t" << (*it)->getPropertyValue("firstName").first  << endl; 
 
      MultiRelTypeVisitor sv9;
      sv9.setFilter(Filters[0]);
      sv9.setRangeFilter(Filters[1]);
      sv9.setDepth(1);
      sv9.setDepthToCheckRange(1);
      sv9.setPropToCheck(2); //check date 
      unsigned int startVertex = (*it)->getId();
      breadthFirstSearch(graph, startVertex, sv9);
      auto targets = sv9.getTargetsMap();
      TargetsMap.insert(targets.begin(), targets.end());
   }
    for(auto iter = TargetsMap.begin(); iter != TargetsMap.end(); ++iter) {
      cout << "posts/comments " << (*iter).first->getPropertyValue("id").first << "\t" << (*iter).first->getPropertyValue("creationDate").first << " made by person " << (*iter).second->getPropertyValue("id").first << "\t" <<  (*iter).second->getPropertyValue("firstName").first << endl;
    }
  }
};

class LdbcQuery10 : public Query {
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 11================\n";
    Filter tmpFilter[7];
    traverseThroughMultiRelType("KNOWS", "", tmpFilter[0]);
    traverseThroughMultiRelType("KNOWS", "", tmpFilter[1]);
    traverseThroughMultiRelType("POST_HAS_CREATOR", tmpFilter[2]); 
    traverseThroughMultiRelType("POST_HAS_TAG", tmpFilter[3]); 
    traverseThroughMultiRelType("HAS_INTEREST", tmpFilter[4]); 
    tmpFilter[5].setValueRange("birthday", "",""); //functions to support this
    tmpFilter[6].setProperty("id", startVertex);

    SimilarityVisitor v10;
    v10.setDepth(5);
    v10.setDepthToCheckRange(2);
    v10.setDepthToCheckVertexProperty(5);
    v10.setFilter(tmpFilter[0]);
    v10.setFilter(tmpFilter[1]);
    v10.setFilter(tmpFilter[2]);
    v10.setFilter(tmpFilter[3]);
    v10.setFilter(tmpFilter[4]);
    v10.setRangeFilter(tmpFilter[5]);
//    v10.setVertexFilter(tmpFilter[6]);

  }
};

class LdbcQuery11 : public Query {
public:
  typedef pair<EdgePointer, VertexPointer> MapPair;
  typedef multimap<VertexPointer, MapPair> MatchMapType; 
  typedef pair<VertexPointer, VertexPointer> ReturnMapPair;
  typedef vector<MatchMapType> ReturnTargetsType;
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex ) {
    cout << "===============Query 11================\n";
    Filter tmpFilter[2];
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[0]);
    traverseThroughTypeAndDirection("KNOWS", "", tmpFilter[1]);

    SingleRelTypeVisitor v11;
    v11.setFilter(tmpFilter[0]);
    v11.setFilter(tmpFilter[1]);
    v11.setDepth(2);
    breadthFirstSearch(graph, startVertex, v11);
    auto target = v11.getVertexTargetList();
    cout << startVertex << " is connected with " << target.size() << " friends and friends of friends" << endl;
    Filter Filters[4];
    traverseThroughMultiRelType("WORKS_AT",Filters[0]); 
    traverseThroughMultiRelType("ORGANISATION_IS_LOCATED_IN",Filters[1]); 
    Filters[2].setValueRange("workFrom", "", "2018"); 
    Filters[3].setProperty("name", "Angola");  

    MatchMapType TargetsMap;
    for(auto it = target.begin(); it != target.end(); ++it) {
      cout <<"friend " << (*it)->getId() << "\t" << (*it)->getPropertyValue("id").first << "\t" << (*it)->getPropertyValue("firstName").first  << endl; 
   
      VertexPropertyVisitor sv11;
      sv11.setFilter(Filters[0]);
      sv11.setFilter(Filters[1]);
      sv11.setRangeFilter(Filters[2]);
      sv11.setVertexFilter(Filters[3]);
      sv11.setDepth(2);
      sv11.setDepthToCheckRange(1);
      sv11.setDepthToCheckVertexProp(2);
      unsigned int startVertex = (*it)->getId();
      breadthFirstSearch(graph, startVertex, sv11);
      auto targets = sv11.getMatchMap();
      TargetsMap.insert(targets.begin(), targets.end());
    }
      FixedString key("workFrom");
      for(auto iter = TargetsMap.begin(); iter != TargetsMap.end(); ++iter) {
        cout << (*iter).first->getPropertyValue("firstName").first << " works at "  << (*iter).second.second->getPropertyValue("id").first << " from " << (*iter).second.first->getPropertyValue(key).first << endl;
      }
  }
};

class LdbcQuery13 : public Query {
public:
  void runQuery(Graph & graph, VertexDescriptor startVertex, VertexDescriptor endVertex ) {
    PathVisitor v13;
    v13.setEndVertex(endVertex);
    breadthFirstSearch(graph, startVertex, v13);
    auto target = v13.getVertexTargetList();
    if(target.empty())
      cout << startVertex << " and " <<  endVertex <<" are not connected" << endl;
    else {
      cout << "There are  shortest paths of length " << target.size() << "  from " << startVertex << " to " << endVertex << endl;
      for(auto it = target.begin(); it != target.end(); ++it) {
        cout <<"Vertex " << (*it)->getId() << "\t" << (*it)->getPropertyValue("id").first << (*it)->getPropertyValue("firstName").first<< endl;
      }
    }
  }

};


#endif /*_LDBCQUERYDESCRIPTION_H_*/