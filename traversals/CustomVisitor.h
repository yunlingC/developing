//===-- traversals/CustomVisitor.h - Customed visitor class ----------*- C++ -*-===//
//
//                     CAESR Graph Database 
//
// TODO: LICENSE
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This is the customed class for Graph visitors.
///
//===----------------------------------------------------------------------===//

#ifndef _CUSTOMVISITOR_H_
#define _CUSTOMVISITOR_H_

#include <queue>

#include "Visitor.h"
#include "Utilities.h"

using namespace std;

/// visitor to deal with query of selection : query1 and query 4
class SelectionVisitor: public Visitor {
public:
  typedef pair<FixedString, bool> ReturnValueType;
  typedef GraphType::VertexPointer VertexPointer;
  typedef std::vector<VertexPointer> VertexTarget;
public:
  SelectionVisitor() { }

  virtual void setFilter(Filter & f) {
    _filter = f;
  }

  Filter & getFilter() { 
    return _filter;
  }

  VertexTarget & getVertexTargetList() {
    return _VertexTargetList;
  }

  virtual bool visitVertex(VertexPointer vp) {
    bool VertexMatch = checkProperty<ReturnValueType>(vp, getFilter());
    if(VertexMatch == true) 
      _VertexTargetList.push_back(vp);
    return TerminateAtVertex(1, _VertexTargetList);
  }

  virtual bool  scheduleEdge(EdgePointer ep ) {
    return checkType(ep, getFilter() );
  }


  virtual void dumpTarget() {
    dumpVertexTarget(_VertexTargetList);
  }

private:
  VertexTarget _VertexTargetList;
  Filter _filter;
};

/// visitor to deal with adjacency query : from query 2 to query 3
class AdjacencyVisitor: public Visitor {
public:
  typedef GraphType::VertexPointer VertexPointer;
  typedef std::vector<VertexPointer> VertexTarget;
public:
  AdjacencyVisitor() { }

  void setFilter(Filter & f) {
    _filter = f;
  }

  Filter & getFilter() {
    return _filter;
  }

  VertexTarget & getVertexTargetList() {
    return _VertexTargetList;
  }

  virtual bool visitVertex(VertexPointer vertex) {
    if(_depthList.find(vertex) != _depthList.end())
      if(_depthList[vertex] >= 1)
        return true;
  }

  virtual bool scheduleEdge(EdgePointer edge) {
    return checkType(edge, _filter);
  }

  virtual bool scheduleBranch(VertexPointer first, EdgePointer edge, VertexPointer second) {
    bool TypeMatch = checkType(edge, _filter);
    if(TypeMatch == true) {
      _VertexTargetList.push_back(second);
    }
    computeDepth(first, edge, second, _depthList);
    return TerminateAtDepth(1, _depthList);
  }

private:
  Filter _filter;
  DepthList _depthList;
  VertexTarget _VertexTargetList;
};


/// visitor to deal with multiple typefilters: from query 5 to 7 
class ReachabilityVisitor : public Visitor {
public:
  typedef GraphType::VertexPointer VertexPointer;
  typedef std::vector<VertexPointer> VertexTarget;
  typedef std::vector<VertexPointer> VertexPath;
  typedef std::queue<VertexPath>  PathQueue;
public:
  ReachabilityVisitor () { }

  virtual void setFilter(Filter & f) {
    _filterList.push_back(f);
  }

  virtual void setDepth(unsigned int depth) {
    _depthSetting = depth;
  }

  VertexTarget & getVertexTargetList() {
    return _vertexTargetList;
  }

  virtual bool visitStartVertex(VertexPointer startVertex ) {
    VertexPath newPath;
    newPath.push_back(startVertex);
    _pathQueue.push(newPath);
  }


  virtual bool discoverVertex(VertexPointer vertex) {
    return true;
  }

  virtual bool visitVertex(VertexPointer vertex) {
    _prevPath = _pathQueue.front();
    _pathQueue.pop();
    if(_prevPath.size() > _depthSetting) {
      _vertexTargetList.push_back(_prevPath.at(_depthSetting));
      while( !_pathQueue.empty()) {
        auto path = _pathQueue.front(); _pathQueue.pop();
        bool unique = true;
        for (auto it = _vertexTargetList.begin() ; it != _vertexTargetList.end(); ++ it) {/// do not store repeated veretx
          if ( *it == path.at(_depthSetting)) {
            unique = false;
            break;
          }
        }
        if(unique == true) { 
          _vertexTargetList.push_back(path.at(_depthSetting));
        }//end if
      }
      return true;
    }
  }


  virtual bool visitDirection(VertexPointer target, EdgePointer edge) {
    return _direcMatch;

  }

  virtual bool scheduleEdge(EdgePointer edge) {
    return _typeMatch;
  }

  virtual bool scheduleBranch(VertexPointer first, EdgePointer edge, VertexPointer second) {
    unsigned int depthSecond;
    if(_prevPath.back() == first) {
      depthSecond = _prevPath.size();
    }

    Filter filter;
    if(depthSecond > 0)
      filter = _filterList[depthSecond-1];
    else
      filter.setDefault();
   
    _typeMatch = checkType(edge, filter);
    _direcMatch = checkDirection(second, edge, filter);
    if(_typeMatch && _direcMatch) {
      VertexPath newPath = _prevPath;
      newPath.push_back(second);
      _pathQueue.push(newPath);
    }
    return false;
  }

private:
  unsigned int  _typeMatch;
  unsigned int  _direcMatch;
  unsigned int  _depthSetting;
  std::vector<Filter> _filterList;
  VertexTarget _vertexTargetList;
  VertexPath   _prevPath;
  PathQueue  _pathQueue;
};


class PathVisitor : public Visitor {
public:
  typedef GraphType::VertexPointer VertexPointer;
  typedef GraphType::VertexDescriptor VertexDescriptor;
  typedef std::vector<VertexPointer> VertexTarget;
  typedef std::vector<VertexPointer> VertexPath;
  typedef std::queue<VertexPath>  PathQueue;
public:
  PathVisitor () { }

  void setEndVertex(VertexDescriptor vertex) {
    _endVertex =  vertex;
  }

  VertexTarget & getVertexTargetList() {
    return _vertexTargetList;
  }

  virtual bool visitStartVertex(VertexPointer startVertex ) {
    _startVertex = startVertex->getId();
    VertexPath newPath;
    newPath.push_back(startVertex);
    _pathQueue.push(newPath);
  }

  virtual bool visitVertex(VertexPointer vertex) {
    _prevPath = _pathQueue.front();
    _pathQueue.pop();
    return false;
  }

  virtual bool scheduleTree(VertexPointer first, EdgePointer edge, VertexPointer second) {
      VertexPath newPath = _prevPath;
      newPath.push_back(second);
      _pathQueue.push(newPath);
      if(second->getId() == _endVertex) {
        _vertexTargetList = newPath;
        return true;
      }
    return false;
  }
  
private:
  VertexDescriptor _endVertex;
  VertexDescriptor _startVertex;
  VertexTarget _vertexTargetList;
  VertexPath   _prevPath;
  PathQueue  _pathQueue;
};

class PatternVisitor : public Visitor {
public:
  typedef GraphType::VertexPointer VertexPointer;
  typedef GraphType::VertexDescriptor VertexDescriptor;
  typedef std::vector<VertexPointer> VertexTarget;
  typedef std::vector<VertexPointer> VertexPath;
  typedef std::queue<VertexPath>  PathQueue;
public:
  PatternVisitor () { }

  void setFilter(Filter & f) {
    _filterList.push_back(f);
  }

  void setDepth(unsigned int depth) {
    _depthSetting = depth;
  }
  
  void setEndVertex(VertexDescriptor vertex) {
    _endVertex =  vertex;
  }

  Filter & getFilter() {
    return _directionFilter;
  }

  VertexTarget & getVertexTargetList() {
    return _vertexTargetList;
  }

  virtual bool visitStartVertex(VertexPointer startVertex ) {
    VertexPath newPath;
    newPath.push_back(startVertex);
    _pathQueue.push(newPath);
  }

  virtual bool visitVertex(VertexPointer vertex) {
    _prevPath = _pathQueue.front();
    _pathQueue.pop();
    if(_prevPath.size() > _depthSetting) {
      if(_prevPath.at(_depthSetting)->getId() == _endVertex) 
        _vertexTargetList.push_back(_prevPath.at(1));
      while( !_pathQueue.empty()) {
        auto path = _pathQueue.front(); _pathQueue.pop();
        if( path.at(_depthSetting)->getId() == _endVertex) {
          bool unique = true;
          for (auto it = _vertexTargetList.begin() ; it != _vertexTargetList.end(); ++ it) {/// do not store repeated veretx
            if ( *it == path.at(1)) {
              unique = false;
              break;
            }
          }
          if(unique == true) 
          _vertexTargetList.push_back(path.at(1));
        }
      }
      return true;
    }
  }

  virtual bool visitDirection(VertexPointer target, EdgePointer edge) {
    return _direcMatch;

  }

  virtual bool scheduleEdge(EdgePointer edge) {
    return _typeMatch;
  }

  virtual bool scheduleBranch(VertexPointer first, EdgePointer edge, VertexPointer second) {
    unsigned int depthSecond;
    if(_prevPath.back() == first) {
      depthSecond = _prevPath.size();
    }

    Filter filter;
      if(depthSecond > 0)
        filter = _filterList[depthSecond-1];
    else
      filter.setDefault();
   
    _typeMatch = checkType(edge, filter);
    _direcMatch = checkDirection(second, edge, filter);
    if(_typeMatch && _direcMatch )
    if(_typeMatch && _direcMatch) {
      VertexPath newPath = _prevPath;
      newPath.push_back(second);
      _pathQueue.push(newPath);
    }
    return false;
  }

private:
  unsigned int  _typeMatch;
  unsigned int  _direcMatch;
  unsigned int  _depthSetting;
  Filter _directionFilter;
  VertexDescriptor _endVertex;
  std::vector<Filter> _filterList;
  VertexTarget _vertexTargetList;
  VertexPath   _prevPath;
  PathQueue  _pathQueue;
};


class DFSVisitor: public ReachabilityVisitor {
public:
  typedef GraphType::VertexPointer VertexPointer;
  typedef std::map<VertexPointer,  unsigned int> VertexTarget;
  typedef std::pair<VertexPointer, unsigned int> VertexTargetPair;
public:

  virtual void setFilter(Filter & f) {
    _filterList.push_back(f);
  }

  virtual bool visitStartVertex(VertexPointer startVertex ) {
    _startVertex = startVertex;
    _depthList.insert(VertexTargetPair(startVertex, 0));
  }


  virtual VertexTarget & getVertexTargetList() {
    return _VertexTargetList;
  }

  virtual void setDepth(unsigned int depth) {
    _depthSetting = depth;
  }

  virtual bool visitVertex(VertexPointer vertex) {
    if( checkMaxDepth(_depthList) >= _depthSetting) {
     for( auto it = _depthList.begin(); it != _depthList.end(); ++it ) {
       if( (*it).second == _depthSetting) {
         _VertexTargetList.insert(VertexTargetPair((*it).first, true));
       }
     }
    }
  }

  virtual bool scheduleBranch(VertexPointer first, EdgePointer edge, VertexPointer second) {
    _typeMatch = false;
    _direcMatch = false;
    for(auto it = _depthList.equal_range(first).first; it != _depthList.equal_range(first).second; ++it) {
      if((*it).first == first) {
        if((*it).second < _depthSetting) {
          Filter filter = _filterList[(*it).second];
          if(_depthList.count(second) <= _depthSetting) {
          if(checkType(edge, filter) == true)
            _typeMatch = true;
          if(checkDirection(second, edge, filter) == true)
            _direcMatch = true;
          if(_typeMatch && _direcMatch)
            recordDepth(first, edge, second, _depthList);
          } //end if 
        }
      }
    }
   return false;
  }

  virtual bool visitDirection(VertexPointer target, EdgePointer edge) {
    return _direcMatch;
  }

  virtual bool scheduleEdge(EdgePointer edge) {
    return _typeMatch;
  }

private:
  Filter _filter;
  MultiDepthList   _depthList;
  VertexPointer    _startVertex;
  unsigned int     _typeMatch;
  unsigned int     _direcMatch;
  unsigned int     _depthSetting;
  VertexTarget     _VertexTargetList;
  std::vector<Filter> _filterList;
};

class DFSPatternVisitor : public PatternVisitor {
public:
  typedef GraphType::VertexPointer VertexPointer;
  typedef GraphType::VertexDescriptor VertexDescriptor;
  typedef std::vector<VertexPointer> VertexTarget;
  typedef std::vector<VertexPointer> VertexPath;
  typedef std::vector<VertexPath>  PathStack;
public:

  void setFilter(Filter & f) {
    _filterList.push_back(f);
  }

  void setDepth(unsigned int depth) {
    _depthSetting = depth;
  }
  
  void setEndVertex(VertexDescriptor vertex) {
    _endVertex =  vertex;
  }

  VertexTarget & getVertexTargetList() {
    return _vertexTargetList;
  }

  virtual bool visitStartVertex(VertexPointer startVertex ) {
    _startVertex = startVertex;
    VertexPath newPath;
    newPath.push_back(startVertex);
    _pathStack.push_back(newPath);
  }

  virtual bool visitVertex(VertexPointer vertex) {
    _prevPath = _pathStack.back();
    _pathStack.pop_back();
    if(_prevPath.size() > _depthSetting) {
      if(_prevPath.at(_depthSetting)->getId() == _endVertex) {
        bool unique = true;
        for (auto it = _vertexTargetList.begin() ; it != _vertexTargetList.end(); ++ it) {/// do not store repeated veretx
          if ( *it == _prevPath.at(1)) {
            unique = false;
            break;
          }
        }
        if(unique == true) {
          _vertexTargetList.push_back(_prevPath.at(1));
        }
      }
    }
    return false;
  }

  virtual bool discoverVertex(VertexPointer vertex) {
    return true; 
  }

  virtual bool visitDirection(VertexPointer target, EdgePointer edge) {
    return _direcMatch;

  }

  virtual bool scheduleEdge(EdgePointer edge) {
    return _typeMatch;
  }

  virtual bool scheduleBranch(VertexPointer first, EdgePointer edge, VertexPointer second) {

    _typeMatch = false;
    _direcMatch = false;
    if(_prevPath.size() > _depthSetting) {
      return false;
    }
    unsigned int firstDepth;
    if(_prevPath.back() == first) {
      firstDepth = _prevPath.size() - 1;
    }

    Filter filter;
      if(firstDepth >= 0 && firstDepth < _depthSetting)
        filter = _filterList[firstDepth];
    else
      filter.setDefault();

   if(second != _startVertex) {
      _typeMatch = checkType(edge, filter);
      _direcMatch = checkDirection(second, edge, filter);
  
    if(_typeMatch && _direcMatch) {
      VertexPath newPath = _prevPath;
      newPath.push_back(second);
      _pathStack.push_back(newPath);
    }
   }
    return false;
  }

private:
  unsigned int  _typeMatch;
  unsigned int  _direcMatch;
  unsigned int  _depthSetting;
  VertexPointer _startVertex;
  VertexDescriptor _endVertex;
  std::vector<Filter> _filterList;
  VertexTarget _vertexTargetList;
  VertexPath   _prevPath;
  PathStack _pathStack;
};


class DFSPathVisitor : public Visitor {
public:
  typedef GraphType::VertexPointer VertexPointer;
  typedef GraphType::VertexDescriptor VertexDescriptor;
  typedef std::vector<VertexPointer> VertexTarget;
  typedef std::vector<VertexPointer> VertexPath;
  typedef std::vector<VertexPath>  PathStack;
  typedef std::multimap<unsigned int, VertexPath> PathMap;
  typedef std::pair<unsigned int, VertexPath> PathPair;
public:

  void setFilter(Filter & f) {
    _filterList.push_back(f);
  }

  void setDepth(unsigned int depth) {
    _depthSetting = depth;
  }
  
  void setEndVertex(VertexDescriptor vertex) {
    _endVertex =  vertex;
  }

  VertexTarget & getVertexTargetList() {
    return _vertexTargetList;
  }

  PathMap & getPathList() {
    return _pathMap;
  }

  virtual bool visitStartVertex(VertexPointer startVertex ) {
    _startVertex = startVertex;
    _tmpMinDepth = 500;
    _turnFlag = false;
    VertexPath newPath;
    newPath.push_back(startVertex);
    _pathStack.push_back(newPath);
  }

  virtual bool visitVertex(VertexPointer vertex) {
    _prevPath = _pathStack.back();
    _pathStack.pop_back();
    if(_prevPath.size() >=  _tmpMinDepth) {
      _turnFlag = true; //prune not to go on with this branch
    } else 
      _turnFlag = false;
    return false;
  }
  
  virtual bool visitDirection(VertexPointer target, EdgePointer edge) {
    return _direcMatch;
  }

  virtual bool scheduleEdge(EdgePointer edge) {
    return _typeMatch;
  }

  virtual bool scheduleBranch(VertexPointer first, EdgePointer edge, VertexPointer second) {

    _typeMatch = false;
    _direcMatch = false;
    /// if the depth of this branch already exceeds the shortest path, then prune
    if(_turnFlag == true) {
      return true;
    }

    VertexPath newPath = _prevPath;
    newPath.push_back(second);
    /// if find the endVertex, add to map and never visit this branch and following branches again
    if(second->getId() == _endVertex) {
      if (newPath.size() <= _tmpMinDepth) {
        if(newPath.size() < _tmpMinDepth)
          _pathMap.clear();
        _pathMap.insert(PathPair(newPath.size(), newPath));
        _tmpMinDepth = newPath.size();
      }
      return true;
    } else {
      _typeMatch = true;
      _direcMatch = true;
      return false;
    }
  }

  virtual bool scheduleTree(VertexPointer first, EdgePointer edge, VertexPointer second){

    if(_typeMatch && _direcMatch) {
      VertexPath newPath = _prevPath;
      newPath.push_back(second);
      _pathStack.push_back(newPath);
    }
    return false;
  }

private:
  bool _turnFlag;
  unsigned int  _tmpMinDepth;
  unsigned int  _typeMatch;
  unsigned int  _direcMatch;
  unsigned int  _depthSetting;
  VertexPointer _startVertex;
  VertexDescriptor _endVertex;
  std::vector<Filter> _filterList;
  VertexTarget _vertexTargetList;
  PathMap     _pathMap;
  VertexPath   _prevPath;
  PathStack _pathStack;
};


#endif /**_CUSOTMVISITOR_H_*/
