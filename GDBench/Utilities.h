//===-- traversals/Utilities.h - functions to custom visitors---*- C++ -*-===//
//
//                     CAESR Graph Database 
//
// TODO: LICENSE
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This is the function utilities for Graph visitors.
///
//===----------------------------------------------------------------------===//
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <stack>
#include <time.h>
#include <boost/algorithm/string.hpp>

#include "Filter.h"
#include "GraphType.h"

typedef std::string KeyType;
typedef std::string ValueType;
typedef std::string Type;
typedef std::string RelType;
typedef unsigned int IdType;
typedef std::string Direction;
typedef std::vector<string> AttributesType;
typedef GraphType::VertexPointer VertexPointer;
typedef GraphType::EdgePointer    EdgePointer;
typedef std::map<VertexPointer, unsigned int> DepthList;
typedef std::multimap<VertexPointer, unsigned int> MultiDepthList;
typedef std::map<VertexPointer, unsigned int> DegreeList;
typedef std::vector<VertexPointer> VertexTargetSet;
typedef std::stack<unsigned int > LayerStack;
typedef std::map<unsigned int, VertexTargetSet> LayerMap;


void filtProperty(KeyType Key, ValueType Value, Filter & TraversalFilter){
  TraversalFilter.setKey(Key);
  TraversalFilter.setValue(Value);
}

void filtVertexId(IdType vid , Filter & TraversalFilter) {
  TraversalFilter.setVertexId(vid);
}

void filtEdgeId (IdType eid, Filter & TraversalFilter) {
  TraversalFilter.setEdgeId(eid);
}

void filtType (Type type, Filter & TraversalFilter) {
  TraversalFilter.setType(type);
}

void traverseThroughDirection(Direction direction, Filter & TraversalFilter) {
  TraversalFilter.setDirection(direction);
}

void traverseThroughType(Type type, Filter & TraversalFilter) {
  TraversalFilter.setType(type);
}

void traverseThroughMultiRelType(RelType types, Filter & TypeFilter ) {
  AttributesType attributes;
  boost::split(attributes, types, boost::is_any_of("+"));
  for (auto it = attributes.begin(); it != attributes.end(); it++)
//    cout << "types " << *it ;
  cout << endl;
  TypeFilter.setTypeList(attributes);
}

void traverseThroughTypeAndDirection(Type type, Direction direction, Filter & TraversalFilter) {
  TraversalFilter.setDirection(direction);
  TraversalFilter.setType(type);
}

bool terminateAtVertex(unsigned int targetNum, VertexTargetSet vertexSet) {

    if(vertexSet.size() >= targetNum) 
    {
      return true;
    }
    else // not yet
      return false;
  }


/// compute and return the depth for second
unsigned int computeDepth(VertexPointer first, EdgePointer ep, VertexPointer second, DepthList &dl) {
  typedef pair<VertexPointer, unsigned int> DepthPair;

  unsigned int Depth = 1;
  if (dl.find(first) == dl.end()) {
    dl.insert(DepthPair(first, 0));
    if(dl.find(second) == dl.end())
      dl.insert(DepthPair(second, 1));
    else 
      dl[second] = 1;
  }
  else {
    Depth = dl[first] + 1;
    if(dl.find(second) == dl.end()) {
      dl.insert(DepthPair(second, Depth));
    }
    else {
      if(Depth < dl[second])
        dl[second] = Depth;
    }
  }
  return Depth;
}

void recordDepth(VertexPointer first, EdgePointer ep, VertexPointer second, MultiDepthList &dl) {
  typedef pair<VertexPointer, unsigned int> DepthPair;

  unsigned int Depth = 1;
  if (dl.find(first) == dl.end()) {
    dl.insert(DepthPair(first, 0));
    dl.insert(DepthPair(second, 1));
  }
  else {
    for( auto it = dl.equal_range(first).first; it != dl.equal_range(first).second; ++it ) {
        Depth = (*it).second + 1;
        unsigned int unique = true;
        if(dl.count(second) != 0)
        for(auto iter = dl.equal_range(second).first; iter != dl.equal_range(second).second; ++iter) {
          if((*iter).second == Depth) {
            unique = false;
            break;
          }
        }
          if(unique == true) {
            dl.insert(DepthPair(second, Depth));
          }
    }
  }
  return ;
}

void updateDepth(VertexPointer first, EdgePointer ep, VertexPointer second, DepthList &dl) {
  typedef pair<VertexPointer, unsigned int> DepthPair;

  unsigned int Depth = 1;
  if (dl.find(first) == dl.end()) {
    dl.insert(DepthPair(first, 0));
    if(dl.find(second) == dl.end())
      dl.insert(DepthPair(second, 1));
    else 
      dl[second] = 1;
   
  }
  else {
    Depth = dl[first] + 1;
    if(dl.find(second) == dl.end()) {
      dl.insert(DepthPair(second, Depth));
    }
    else 
      dl[second] = Depth;
  }
  return  ;

}

/// check if the vertex's depth == given depth
bool checkDepth(unsigned int depth, VertexPointer vertex, DepthList & depthList) { if (depthList.find(vertex) != depthList.end()) if(depthList[vertex]  == depth)
      return true;
  return false;
}

unsigned int  checkMaxDepth(MultiDepthList & dl) {
  unsigned int depth = 0;
  for(auto it = dl.begin(); it != dl.end(); it ++) {
    if( (*it).second > depth)
      depth = (*it).second;
  }
  return depth;
}

template<typename ReturnValueType>
bool checkProperty(VertexPointer vertex, Filter &filter) {
  if ((filter.getValue() == "") || (filter.getKey() == "") )
      return true; 
  FixedString value(filter.getValue());
  FixedString key(filter.getKey());

  ReturnValueType rv = vertex->getPropertyValue(key); 
  if((rv.second == false) || (rv.first != value))
  {
      return false;
  }
  return true;
}


int compareTime(time_t time1, time_t time2) {
  auto second = difftime(time1, time2);
  if (second >= 0.0000001) 
    return 1; 
  else  if(second < -0.0000001)
    return -1;
  else 
    return 0;
}

template<typename ReturnValueType, typename GraphElemPointer>
bool checkTimeRange(GraphElemPointer GraphElem, Filter &filter, bool & EqualFlag) {
  char buf[32];
  string times;  
  struct tm  time[2];
  bool cmpResult[2] = {false, false};
  EqualFlag = false;
  vector<string> attributes;
  for (auto i = 0; i < 2; i++) {
    times = filter.getValueRange().at(i);
    if(times == "")
      cmpResult[i] = true; 
    else {
      boost::split(attributes, times, boost::is_any_of("T"));
      boost::split(attributes, attributes[1], boost::is_any_of("."));
      memset(&time[i], 0, sizeof(struct tm));
      strptime(attributes[0].c_str(), "%H:%M:%S", &time[i]); 
      strftime(buf, sizeof(buf), "%H:%M:%S", &time[i]);
    } //END_ELSE
  } //END_FOR
  auto timeProp = GraphElem->getPropertyValue(filter.getKey());                          
  if(timeProp.second == false) {
    cout << "Error: Failed to find time property\n";
    return false;   /// cannot find time property ;
  }
  else  {
    struct tm TimeToCheck;
    auto timeString = timeProp.first.std_str();
    boost::split(attributes, timeString, boost::is_any_of("T"));
    boost::split(attributes, attributes[1], boost::is_any_of("."));
      memset(&TimeToCheck, 0, sizeof(struct tm));
    strptime(attributes[0].c_str(), "%H:%M:%S", &TimeToCheck);
    strftime(buf, sizeof(buf), "%H:%M:%S", &TimeToCheck);
    auto t = mktime(&TimeToCheck); 
    if (cmpResult[0] != true) {
      auto t1 = mktime(&time[0]); 
      auto cmp = compareTime(t, t1);
      if( cmp >=  0) {
        if ( cmp == 0) 
          EqualFlag = true;
        cmpResult[0] = true; 
      }
    }
    if (cmpResult[1] != true) {
      auto t2 = mktime(&time[1]);
      auto cmp2 = compareTime(t2, t);
      if(cmp2 >= 0) {
        if (cmp2 == 0)
          EqualFlag = true;
        cmpResult[1] = true; 
      }
    }
    return (cmpResult[0] && cmpResult[1]); 
  }
 }

template<typename GraphElemType>
bool checkYearRange(GraphElemType elem, Filter & filter, bool & EqualFlag) {
  EqualFlag = false;
  bool cmpResult[2] = {false, false};
  unsigned int Year[2] = {0, 0};
  for(auto i = 0; i < 2; i++){
    if(filter.getValueRange()[i] == "")
      cmpResult[i] = true;
    else  {
      Year[i] = stoi(filter.getValueRange()[i]);
    }
  }
  if(elem->getPropertyValue(filter.getKey()).second == false)
    return false;
  auto YearProp = stoi(elem->getPropertyValue(filter.getKey()).first.std_str());
  if(cmpResult[0] == false)
    if( YearProp >= Year[0]) {
      if (YearProp == Year[0])
        EqualFlag = true;
      cmpResult[0] = true;
    }
  if(cmpResult[1] == false) {
    if( YearProp <= Year[1])
      cmpResult[1] = true;
  }
  return (cmpResult[0] && cmpResult[1]);

  }

template<typename ReturnValueType, typename GraphElemPointer>
bool checkDateRange(GraphElemPointer GraphElem, Filter &filter, bool & EqualFlag) {
  char buf[32];
  string times;  
  struct tm date[2];
  EqualFlag = false;
  bool cmpResult[2] = {false, false};
  vector<string> attributes;
  for (auto i = 0; i < 2; i++) {
    times = filter.getValueRange().at(i);
    if(times == "") 
      cmpResult[i] = true;
    else {
      boost::split(attributes, times, boost::is_any_of("T"));
      memset(&date[i], 0, sizeof(struct tm));
      strptime(attributes[0].c_str(), "%Y-%m-%d", &date[i]);
      strftime(buf, sizeof(buf), "%Y-%m-%d", &date[i]);
    }
  }
  auto timeProp = GraphElem->getPropertyValue(filter.getKey());                          
  if(timeProp.second == false)
    return false;   /// cannot find time property ;
  else  {
    struct tm DateToCheck;
    auto dateString = timeProp.first.std_str();
    boost::split(attributes, dateString, boost::is_any_of("T"));
     memset(&DateToCheck, 0, sizeof(struct tm));
    strptime(attributes[0].c_str(), "%Y-%m-%d", &DateToCheck);
    strftime(buf, sizeof(buf), "%Y-%m-%d", &DateToCheck);
    auto d = mktime(&DateToCheck);
    if(cmpResult[0] != true) {
      auto d1 = mktime(&date[0]);
      auto cmp = compareTime(d, d1);
      if (cmp >= 0) {
        if(cmp == 0) 
          EqualFlag = true;
        cmpResult[0] = true; 
      }
    }
    if(cmpResult[1] != true) {
      auto d2 = mktime(&date[1]);
      auto cmp2 = compareTime(d2, d);
      if(cmp2 >= true) {
        if(cmp2 == true)
          EqualFlag = true;
        cmpResult[1] = true; 
      }
    }
    return (cmpResult[0] && cmpResult[1]); 
  }
 }

template<typename GraphElemType>
bool checkRange(unsigned int opt, GraphElemType elem, Filter & RangeFilter, bool & EqualFlag) {
  typedef std::pair<FixedString, bool> ReturnValueType;
  switch(opt) {
          case 1:
            return checkTimeRange<ReturnValueType, GraphElemType>(elem, RangeFilter, EqualFlag);
          case 2:
            return checkDateRange<ReturnValueType, GraphElemType>(elem, RangeFilter, EqualFlag);
          case 3:
            return checkYearRange<GraphElemType>(elem, RangeFilter, EqualFlag);
          case 0:
            return true;
          default:
            return true;
        }//switch 
}

void dumpVertexTarget(VertexTargetSet & VertexTargetList) {

  for (auto it = VertexTargetList.begin(); it != VertexTargetList.end(); ++it) {
    cout << "Vertex: "<< (*it)->getId() << endl;
  }
}

bool TerminateAtDepth(unsigned int depth, DepthList & depthlist) {
  for(auto it = depthlist.begin(); it != depthlist.end(); ++ it) {
    if(it->second > depth)
      return true;
  }
    return false;
}

bool checkType(EdgePointer edge, Filter &filter) {
    if (filter.getType() == "")
      return true;
    FixedString type(filter.getType());
    if (edge->getType() == type)
    {
      return true;
    }
    else  {
      return false;
    }
}

bool checkMultiRelType(EdgePointer edge, Filter & filter) {
  auto typeMatch = false;   
  if(filter.getTypeList().empty())
    return true;
  for (auto it = filter.getTypeList().begin(); it != filter.getTypeList().end(); ++it) {
   if (*it == "") {
      return true;
   }
    FixedString type(*it);
    if (edge->getType() == type)
    {
      return true;
    }
    else  {
      typeMatch = false;
    }
  }
  return typeMatch;
}

/// check if the direction of vertex to edge is 
bool checkDirection(VertexPointer vertex, EdgePointer edge, Filter & filter ) {
    int direction = 0;

    if(filter.getDirection() == "" )
      return true;

    if ( filter.getDirection() == "out")
      direction = 1;     //first vertex => outEdge for vertex;
    else if ( filter.getDirection() == "in")
      direction = 2;     //second vertex => inEdge for vertex;
    else 
      cout <<"Err: Irrelevant edges.\n";

    switch(direction) {
    case 1:
      if (edge->getFirstId() == vertex->getId()) {
        return false;
      }
      break;

    case 2:
      if (edge->getSecondId() == vertex->getId()) { 
        return false;
      }
      break;

    default:
      return true;
    }
    return true;
}


void computeOutDegree(VertexPointer vertex, EdgePointer edge, DegreeList &list) {
  typedef pair<VertexPointer, unsigned int> DegreePair; 

  if( list.find(vertex) == list.end() ) {
    list.insert(DegreePair(vertex, 0));
  }

  if ( edge->getFirstId() == vertex->getId() ) {
    list[vertex] = list[vertex] + 1;
  }

}

void computeInDegree(VertexPointer vertex, EdgePointer edge, DegreeList &list) {
  typedef pair<VertexPointer, unsigned int> DegreePair; 

  if( list.find(vertex) == list.end() ) {
    list.insert(DegreePair(vertex, 0));
  }

  if ( edge->getFirstId() == vertex->getId() ) {
    list[vertex] = list[vertex] + 1;
  }
}

#endif /**_UTILITIES_H_*/
