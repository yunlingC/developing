//===-- traversals/UpdateVisitor.h - Customed visitor class -----*- C++ -*-===//
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

#ifndef _LDBCUPDATEVISITOR_H_
#define _LDBCUPDATEVISITOR_H_

#include <queue>
#include <set>

#include "ConcurrentVisitor.h"
#include "Utils.h"

using namespace std;

class InsertVisitor : public ConcurrentVisitor {
public:
  typedef pair<FixedString, bool> ReturnValueType;
//  typedef LocksManager & LockManagerType;
protected:
  LockManagerType & LockManager;
  GraphType & graph;
public:

  InsertVisitor (LockManagerType & lm, GraphType & graph) :
                LockManager(lm), graph (graph) {}

  virtual bool visitVertex(VertexPointer vp) {
//    bool GetLock = false;
//    while ( !GetLock ) {
//      GetLock = getVertexLock(vp->getId(), Pp, SH);
//    }
    bool VertexMatch = checkProperty<ReturnValueType>(vp, getFilter());
//    releaseVertexLock(vp->getId(), Pp, SH);
    ///TODO bug here. not consistent
    //there is nothing like upgrade lock from sh to ex directly without deadlock from C++
    //what should we do? Getting the ex at the beginning? too slow...
    //Give up on lock of pp because this is id. No one changes id.

    if( !VertexMatch )  {
      return false;
    }

    _VertexTargetList.push_back(vp);
   
    auto VertexId = graph.addVertex();
    LockManager.addToVertexLockMap(VertexId);
    std::cout << "add one vertex " << VertexId << "\n";

    auto SourceId = vp->getId();
    ///TODO should be moved to graphType.h
    auto VLELock = false;
    auto VNELock = false;
    auto SLELock = false;
    auto SNELock = false;

    while( !VLELock || !VNELock || !SLELock || !SNELock) {
      if( VLELock ) 
        LockManager.releaseVertexLock(VertexId, LE, EX);
      if( VNELock )
        LockManager.releaseVertexLock(VertexId, NE, EX);
      if( SLELock ) 
        LockManager.releaseVertexLock(SourceId, LE, EX);
      if( SNELock )
        LockManager.releaseVertexLock(SourceId, NE, EX);


      ///TODO should there be sleep or wait time here?
      VLELock = LockManager.getVertexLock(VertexId, LE, EX);
      VNELock = LockManager.getVertexLock(VertexId, NE, EX);
      SLELock = LockManager.getVertexLock(SourceId, LE, EX);
      SNELock = LockManager.getVertexLock(SourceId, NE, EX);
    }

    auto EdgeId = graph.addEdge(SourceId, VertexId, "KNOWS");
    LockManager.addToEdgeLockMap(EdgeId);
    cout << "Insert: Success\n";

    LockManager.releaseVertexLock(VertexId, LE, EX);
    LockManager.releaseVertexLock(VertexId, NE, EX);
    LockManager.releaseVertexLock(SourceId, LE, EX);
    LockManager.releaseVertexLock(SourceId, NE, EX);
    return true;
  }

  virtual bool scheduleEdge(EdgePointer edge) {
    auto ReturnValue = false;
    auto Lock = false; 
//    cout << "check locks number " <<  LockManager.getVertexLockMap().size() << "\n";
    while( !Lock ) {
      Lock = LockManager.getEdgeLock(edge->getId(), Pp, SH);
    }

    ReturnValue = checkType(edge, _Filter);
    LockManager.releaseEdgeLock( edge->getId(), Pp, SH);

    return ReturnValue;
  }

  virtual void dumpTarget() {
    dumpVertexTarget( _VertexTargetList );
  }
};

class AddVisitor : public ConcurrentVisitor {
public:
  typedef pair<FixedString, bool> ReturnValueType;
  typedef PropertyList<FixedString, FixedString> PropertyListType;
  typedef std::pair<bool, std::pair<std::string, GraphType::PropertyListType> > ReturnBranchType;
//  typedef LocksManager & LockManagerType;
protected:
  LockManagerType & LockManager;
  GraphType & graph;
protected:
  unsigned int VertexId;
  PropertyListType VertexProperty;
public:

  AddVisitor (LockManagerType & lm, GraphType & graph) :
                LockManager(lm), graph (graph) {}

  void setVertexProperty(PropertyListType & pl) {
    VertexProperty = pl;
  }

  virtual void visitStartVertex(VertexPointer Vertex) {
    VertexId = graph.addVertex(VertexProperty);
    LockManager.addToVertexLockMap(VertexId);

    auto NewVertex = graph.getVertexPointer(VertexId);
    std::cout << "add one vertex " << VertexId << "\n";
    if ( NewVertex ) {
      std::cout << "New Vertex info:\n";
    }
  }

//  virtual bool visitVertex( )
  virtual bool visitVertex(VertexPointer first) {
//    //LOCK 1
//    auto EdgeIdLock = false;
//    while ( !EdgeIdLock ) {
//      EdgeIdLock = LockManager.getEdgeLock(edge->getId(), ID, SH);
//    }
//
//    bool TypeMatch = checkTypes(edge, _Filter);
//    if( !TypeMatch )  {
//      LockManager.releaseEdgeLock(edge->getId(), ID, SH);
//      return false;
//    }
//
//    auto SourceId = first->getId();
    auto SourceId = first->getId();
    auto VertexIdLock = false;

    while ( !VertexIdLock ) {
      VertexIdLock = LockManager.getVertexLock(SourceId, ID, SH);
    }

    ReturnBranchType VertexMatch = checkBranch<ReturnValueType>(first, getFilter());
    if( !VertexMatch.first )  {
      LockManager.releaseVertexLock(SourceId, ID, SH);
//      LockManager.releaseEdgeLock(edge->getId(), ID, SH);
      return false;
    }

    ///TODO should be moved to graphType.h
    auto VLELock = false;
    auto VNELock = false;
    auto SLELock = false;
    auto SNELock = false;

    while( !VLELock || !VNELock || !SLELock || !SNELock) {
      if ( VLELock ) 
        LockManager.releaseVertexLock(VertexId, LE, EX);
      if ( VNELock )
        LockManager.releaseVertexLock(VertexId, NE, EX);
      if ( SLELock ) 
        LockManager.releaseVertexLock(SourceId, LE, EX);
      if ( SNELock )
        LockManager.releaseVertexLock(SourceId, NE, EX);

      ///TODO should there be sleep or wait time here?
      std::this_thread::sleep_for(std::chrono::seconds(1));
      cout << "sleep one sec\n" ;

      VLELock = LockManager.getVertexLock(VertexId, LE, EX);
      VNELock = LockManager.getVertexLock(VertexId, NE, EX);
      SLELock = LockManager.getVertexLock(SourceId, LE, EX);
      SNELock = LockManager.getVertexLock(SourceId, NE, EX);
    }

    auto EdgePtr = first->getLastEdge();
    auto EdgeId = EdgePtr->getId();
    auto EdgeLock = false;
    
    ///change FirstNextEdge
    while( !EdgeLock ) { 
      if ( EdgePtr->getFirstVertexPtr()->getId() == first->getId() )
        EdgeLock = LockManager.getEdgeLock(EdgeId, FNE, SH); 
      else
        EdgeLock = LockManager.getEdgeLock(EdgeId, SNE, SH); 
    }

    unsigned int NewEdgeId = 0;
    ///if PropertyList is null for this edge, don't add; otherwise, add propertylist
    if(VertexMatch.second.second.get("null").second != true) {
      NewEdgeId = graph.addEdge(VertexId, SourceId,
        VertexMatch.second.first, VertexMatch.second.second);  
    } else {
      NewEdgeId = graph.addEdge(VertexId, SourceId, 
        VertexMatch.second.first);  
    }

    LockManager.addToEdgeLockMap(NewEdgeId);
    cout << "Insert: Success\n";

    auto NewEdge = graph.getEdgePointer(NewEdgeId);
    if ( NewEdge ) {
      std::cout << "New edge info:\n";
    }

    LockManager.releaseVertexLock(VertexId, LE, EX);
    LockManager.releaseVertexLock(VertexId, NE, EX);
    LockManager.releaseVertexLock(SourceId, LE, EX);
    LockManager.releaseVertexLock(SourceId, NE, EX);
    LockManager.releaseVertexLock(SourceId, ID, SH);
    LockManager.releaseEdgeLock(EdgeId, SNE, SH);
    return false;
  }
};

class AdjacencyExplorer: public ConcurrentVisitor {
public:
  AdjacencyExplorer(LockManagerType & lm) : LockManager(lm) { }

  virtual bool visitVertex(VertexPointer vertex) {
    auto ReturnValue = false;
    auto Lock = false; 
    while (!Lock) {
      Lock = LockManager.getVertexLock(vertex->getId(), ID, SH);
    }
    
    if( _DepthList.find(vertex) != _DepthList.end() ) {
      if( _DepthList[vertex] >= 1 ) {
        ReturnValue = true;
      }
    }

    LockManager.releaseVertexLock(vertex->getId(), ID, SH);
    return ReturnValue;
  }

  virtual bool scheduleEdge(EdgePointer edge) {
    auto ReturnValue = false;
    auto Lock = false; 
    while(!Lock) {
      Lock = LockManager.getEdgeLock(edge->getId(), Pp, SH);
    }

    ReturnValue = checkType(edge, _Filter);
    LockManager.releaseEdgeLock(edge->getId(), Pp, SH);
    return ReturnValue;
  }

  virtual bool visitDirection(VertexPointer target, EdgePointer edge) {
    return _DirecMatch;
  }

  virtual bool scheduleBranch(VertexPointer first, EdgePointer edge, VertexPointer second) {
    _DirecMatch = false;
    auto LockFirst = false; 
    auto LockSecond = false; 
    auto LockEdge = false; 

    //Edge Id lock and Vertex id lock
    while(!LockEdge || !LockFirst || !LockSecond) {
      if( LockEdge ) {
        LockEdge = LockManager.releaseEdgeLock(edge->getId(), ID, SH);
      }

      if( LockFirst ) {
        LockFirst = LockManager.releaseVertexLock(first->getId(), ID, SH);
      }

      if( LockSecond ) {
        LockSecond = LockManager.releaseVertexLock(second->getId(), ID, SH);
      }

      LockEdge = LockManager.getEdgeLock(edge->getId(), ID, SH);
      LockFirst = LockManager.getVertexLock(first->getId(), ID, SH);
      LockSecond = LockManager.getVertexLock(second->getId(), ID, SH);
    }

//    std::cout << "first " << first->getId() << " second " << second->getId() << " edge " << edge->getId() << "\n";

    bool TypeMatch = checkType(edge, _Filter);
    _DirecMatch = checkDirection(second, edge, _Filter);
    std::cout << "direction " << _DirecMatch << " Type " << TypeMatch << "\n";
    if( _DirecMatch && TypeMatch ) {
      std::cout << "Found! vertex id " << second->getId() << "\n";
      _VertexTargetList.push_back(second);
    }

    computeDepth(first, edge, second, _DepthList);
    LockManager.releaseVertexLock(first->getId(), ID, SH);
    LockManager.releaseVertexLock(second->getId(), ID, SH);
    LockManager.releaseEdgeLock(edge->getId(), ID, SH);

    return false;
  }

protected:
  DepthList _DepthList;
  LockManagerType & LockManager;
  bool _DirecMatch;
};


#endif /*_LDBCUPDATEVISTIOR_H_*/