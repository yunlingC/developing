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
#ifndef _UTILS_H_
#define _UTILS_H_

#include "FilterType.h"
#include "GraphType.h"

#include <stack>
#include <time.h>
#include <map>
#include <boost/algorithm/string.hpp>

typedef std::string KeyType;
typedef std::string ValueType;
typedef std::string Type;
typedef std::string RelType;
typedef unsigned int IdType;
typedef std::string Direction;
typedef std::vector<std::string> AttributesType;
typedef GraphType::VertexPointer VertexPointer;
typedef GraphType::EdgePointer    EdgePointer;
typedef std::map<VertexPointer, unsigned int> DepthListType;
typedef std::multimap<VertexPointer, unsigned int> MultiDepthList;
typedef std::map<VertexPointer, unsigned int> DegreeList;
typedef std::vector<VertexPointer> VertexTargetSet;
typedef std::stack<unsigned int > LayerStack;
typedef std::map<unsigned int, VertexTargetSet> LayerMap;


void filtProperty(KeyType key, ValueType value, FilterType & TraversalFilter){
  TraversalFilter.setKey(key);
  TraversalFilter.setValue(value);
}

void filtVertexId(IdType vid , FilterType & TraversalFilter) {
  TraversalFilter.setVertexId(vid);
}

void filtEdgeId (IdType eid, FilterType & TraversalFilter) {
  TraversalFilter.setEdgeId(eid);
}

//TODO set flag 
void filtType (Type type, FilterType & TraversalFilter) {
  TraversalFilter.setType(type);
//  filtTypeFlag = true;
}

void traverseThroughDirection(Direction direction, FilterType & TraversalFilter) {
  TraversalFilter.setDirection(direction);
}

void traverseThroughType(Type type, FilterType & TraversalFilter) {
  TraversalFilter.setType(type);
}

void traverseThroughMultiRelType(RelType types, FilterType & TypeFilter ) {
  AttributesType attributes;
  boost::split(attributes, types, boost::is_any_of("+"));
  for (auto it = attributes.begin(); it != attributes.end(); it++)
  TypeFilter.setTypeList(attributes);
}

void traverseThroughTypeAndDirection(Type type, Direction direction, FilterType & TraversalFilter) {
  TraversalFilter.setDirection(direction);
  TraversalFilter.setType(type);
}

bool TerminateAtVertex(unsigned int targetNum, VertexTargetSet vertexSet) {
    return (vertexSet.size() >= targetNum);
}

/// compute and return the depth for second
unsigned int computeDepth(VertexPointer first, 
                          EdgePointer ep, 
                          VertexPointer second, 
                          DepthListType &dl) {
  typedef std::pair<VertexPointer, unsigned int> DepthPair;

  unsigned int depth = 1;
  if (dl.find(first) == dl.end()) {
    dl.insert(DepthPair(first, 0));
    if(dl.find(second) == dl.end())
      dl.insert(DepthPair(second, 1));
    else 
      dl[second] = 1;
  }
  else {
    depth = dl[first] + 1;
    if(dl.find(second) == dl.end()) {
      dl.insert(DepthPair(second, depth));
    }
    else {
      if(depth < dl[second])
        dl[second] = depth;
    }
  }
  return  depth;
}

void recordDepth(VertexPointer first, 
                 EdgePointer ep, 
                 VertexPointer second, 
                 MultiDepthList &dl) {
  typedef std::pair<VertexPointer, unsigned int> DepthPair;

  unsigned int depth = 1;
  if (dl.find(first) == dl.end()) {
    dl.insert(DepthPair(first, 0));
    dl.insert(DepthPair(second, 1));
  }
  else {
    for( auto it = dl.equal_range(first).first; it != dl.equal_range(first).second; ++it ) {
        depth = (*it).second + 1;
        unsigned int unique = true;
        if(dl.count(second) != 0)
        for(auto iter = dl.equal_range(second).first; iter != dl.equal_range(second).second; ++iter) {
          if((*iter).second == depth) {
            unique = false;
            break;
          }
        }
          if(unique == true) {
            dl.insert(DepthPair(second, depth));
          }
    }
  }
  return ;
}

void updateDepth(VertexPointer first, EdgePointer ep, VertexPointer second, DepthListType &dl) {
  typedef std::pair<VertexPointer, unsigned int> DepthPair;

  unsigned int depth = 1;
  if (dl.find(first) == dl.end()) {
    dl.insert(DepthPair(first, 0));
    if(dl.find(second) == dl.end())
      dl.insert(DepthPair(second, 1));
    else 
      dl[second] = 1;
   
  }
  else {
    depth = dl[first] + 1;
    if(dl.find(second) == dl.end()) {
      dl.insert(DepthPair(second, depth));
    }
    else 
      dl[second] = depth;
  }
  return  ;

}

/// check if the vertex's depth == given depth
bool checkDepth(unsigned int depth, VertexPointer vertex, DepthListType & depthList) {
  if (depthList.find(vertex) != depthList.end())
    return (depthList[vertex]  == depth);
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
bool checkProperty(VertexPointer vertex, FilterType &filter) {
  if ((filter.getValue() == "") || (filter.getKey() == "") )
      return true; 
  FixedString value(filter.getValue());
  FixedString key(filter.getKey());

  ReturnValueType rv = vertex->getPropertyValue(key); 
  if((rv.second == false) || (rv.first != value))
      return false;
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
bool checkTimeRange(GraphElemPointer GraphElem, FilterType &filter, bool & EqualFlag) {
  char buf[32];
  std::string times;  
  struct tm  time[2];
  bool cmpResult[2] = {false, false};
  EqualFlag = false;
  std::vector<std::string> attributes;
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
bool checkYearRange(GraphElemType elem, FilterType & filter, bool & EqualFlag) {
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
  unsigned int YearProp = stoi(elem->getPropertyValue(filter.getKey()).first.std_str());
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
bool checkDateRange(GraphElemPointer GraphElem, FilterType &filter, bool & EqualFlag) {
  char buf[32];
  std::string times;  
  struct tm date[2];
  EqualFlag = false;
  bool cmpResult[2] = {false, false};
  std::vector<std::string> attributes;
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
      unsigned int cmp = compareTime(d, d1);
      if (cmp >= 0) {
        if(cmp == 0) 
          EqualFlag = true;
        cmpResult[0] = true; 
      }
    }
    if(cmpResult[1] != true) {
      auto d2 = mktime(&date[1]);
      unsigned int cmp2 = compareTime(d2, d);
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
bool checkRange(unsigned int opt, GraphElemType elem, FilterType & RangeFilter, bool & EqualFlag) {
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

#ifdef _DEBUG_
void dumpVertexTarget(VertexTargetSet & VertexTargetList) {

  for (auto it = VertexTargetList.begin(); it != VertexTargetList.end(); ++it) {
    std::cout << "Vertex: "<< (*it)->getId() << std::endl;
  }
}
#endif

bool TerminateAtDepth(unsigned int depth, DepthListType & depthlist) {
  for (auto it = depthlist.begin(); it != depthlist.end(); ++ it) {
    if (it->second > depth)
      return true;
  }
    return false;
}

bool checkType(EdgePointer edge, FilterType &filter) {
    if (filter.getType() == "")
      return true;
    FixedString type(filter.getType());
    return (edge->getType() == type);
}

bool checkMultiRelType(EdgePointer edge, FilterType & filter) {
  auto typeMatch = false;   
  if(filter.getTypeList().empty())
    return true;
  for (auto it = filter.getTypeList().begin(); it != filter.getTypeList().end(); ++it) {
    if (*it == "") 
      return true;
    FixedString type(*it);
    if (edge->getType() == type)
      return true;
    else  
      typeMatch = false;
  }
  return typeMatch;
}

/// check the direction of vertex to edge 
bool checkDirection(VertexPointer vertex, EdgePointer edge, FilterType & filter ) {
    unsigned int direction = 0;

    if (filter.getDirection() == "" )
      return true;

    if (filter.getDirection() == "out")
      /// First vertex => outEdge for vertex;
      direction = 1;     
    else if (filter.getDirection() == "in")
      /// Second vertex => inEdge for vertex;
      direction = 2;     

    switch(direction) {
    case 1:
      return (edge->getFirstId() != vertex->getId());

    case 2:
      return (edge->getSecondId() != vertex->getId());

    default:
      return true;
    }
}


void computeOutDegree(VertexPointer vertex, EdgePointer edge, DegreeList &list) {
  typedef std::pair<VertexPointer, unsigned int> DegreePair; 

  if( list.find(vertex) == list.end() ) {
    list.insert(DegreePair(vertex, 0));
  }

  if ( edge->getFirstId() == vertex->getId() ) {
    list[vertex] = list[vertex] + 1;
  }

}

void computeInDegree(VertexPointer vertex, EdgePointer edge, DegreeList &list) {
  typedef std::pair<VertexPointer, unsigned int> DegreePair; 

  if( list.find(vertex) == list.end() ) {
    list.insert(DegreePair(vertex, 0));
  }

  if ( edge->getFirstId() == vertex->getId() ) {
    list[vertex] = list[vertex] + 1;
  }
}

#endif /**_UTILS_H_*/
