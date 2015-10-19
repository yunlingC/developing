//===-- transaction/LocksManager.h - Lock class type -------------*- C++ -*-===//
//
//                     CAESR Graph Database 
//
// TODO: LICENSE
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This is the main class for managing locks .
///
//===----------------------------------------------------------------------===//

#ifndef _LOCKSMANAGER_H_
#define _LOCKSMANAGER_H_

///std=c++14
#include <shared_mutex>
#include <iostream>
#include <thread>
#include <map>

#include "GraphType.h"
#include "Lock.h"

enum MutexType { ID, Pp, LE, NE, FV, SV, FNE, FPE, SNE, SPE };
enum LockType { SH, EX };

class LocksManager {
public:
  typedef std::shared_timed_mutex Mutex;
  typedef std::shared_ptr<Mutex> MutexPointer;
  typedef std::map<unsigned int, VertexLock>  VLockMapType;
  typedef std::map<unsigned int, EdgeLock>    ELockMapType;
  typedef std::pair<unsigned int, VertexLock> VLockPair;
  typedef std::pair<unsigned int, EdgeLock>   ELockPair;
  typedef vector<unsigned int> LockIdListType;
  typedef vector<pair<unsigned int, pair<MutexType, LockType> > > LockListType; 
public:
#ifndef _LOCKING_
  LocksManager() {}
  auto getVertexLock(unsigned int id, MutexType mt, LockType lt) 
    -> bool {
    if(VertexLockMap.find(id) == VertexLockMap.end()) {
      std::cerr  << "Error : No such vertex " << id <<" in map \n";
      ///TODO: shoould be exception here, need to be fixed.
      /**
      *exiting process would give out ownership of this mutex automatically,
      * no need to release it before exiting
      * but consistency state?
      * anyway, it won't happen if we don't delete any lock from manager
      */
      exit(0);
    }
    else {
      MutexPointer mp(nullptr);
      switch(mt) {
        case Pp:
          mp = VertexLockMap[id].getPpMutex();
          break;
        case LE:
          mp = VertexLockMap[id].getLEMutex();
          break;
        case NE:
          mp = VertexLockMap[id].getNEMutex();
          break;
        case ID:
          mp = VertexLockMap[id].getIdMutex();
          break;
        default:
          ///TODO should be exception here
          std::cerr << "ERROR: No such Mutex in VertexLock\n";
          exit(0);
      }//END_SWITCH
      switch (lt) {   
        ///Shared lock
        case SH:
          return mp->try_lock_shared();
        case EX:
          return mp->try_lock(); 
        default:
          ///TODO Should be exception here
          std::cerr << "ERROR: No such Mutex in VertexLock\n";
          exit(0);
      }
    } 
  }

  auto releaseVertexLock(unsigned int id, MutexType mt, LockType lt) 
    -> bool {
    if(VertexLockMap.find(id) == VertexLockMap.end()) {
      std::cerr << "Error : No such vertex " << id <<" in map \n";
      exit(0);
    }
    else {
      MutexPointer mp(nullptr);
      switch(mt) {
        case Pp:
          mp = VertexLockMap[id].getPpMutex();
          break;
        case LE:
          mp = VertexLockMap[id].getLEMutex();
          break;
        case NE:
          mp = VertexLockMap[id].getNEMutex();
          break;
        case ID:
          mp = VertexLockMap[id].getIdMutex();
          break;
        default:
          ///TODO should be exception here
          std::cerr << "ERROR: No such Mutex in VertexLock\n";
          exit(0);
      }//END_SWITCH

      switch (lt) {   
        ///Shared lock
        case SH:
          mp->unlock_shared();
          break;
        case EX:
          mp->unlock(); 
          break;
        default:
          ///TODO Should be exception here
          std::cerr << "ERROR: No such Mutex in VertexLock\n";
          exit(0);
      }
      return true;
    }
  }

  auto getEdgeLock(unsigned int id, MutexType mt, LockType lt) 
    -> bool {
    if (EdgeLockMap.find(id) == EdgeLockMap.end()) {
      std::cerr << "Error : No such edge" << id <<" in map \n";
      exit(0);
    }

    else {
      MutexPointer mp(nullptr);
      switch(mt) {
        case ID:
          mp = EdgeLockMap[id].getIdMutex();
          break;
        case Pp:
          mp = EdgeLockMap[id].getPpMutex();
          break;
        case FV:
          mp = EdgeLockMap[id].getFVMutex();
          break;
        case SV:
          mp = EdgeLockMap[id].getSVMutex();
          break;
        case FNE:
          mp = EdgeLockMap[id].getFNEMutex();
          break;
        case FPE:
          mp = EdgeLockMap[id].getFPEMutex();
          break;
        case SNE:
          mp = EdgeLockMap[id].getSNEMutex();
          break;
        case SPE:
          mp = EdgeLockMap[id].getSPEMutex();
          break;
        default:
          std::cerr << "ERROR: No such Mutex in EdgeLock\n";
          exit(0);
      }//END_SWITCH
      switch (lt) {   
        case SH:
          return mp->try_lock_shared();
        case EX:
          return mp->try_lock(); 
        default:
          std::cerr  << "ERROR: No such Mutex in EdgeLock\n";
          exit(0);
      }
    }
  }

  auto releaseEdgeAll(LockListType & EdgeLocks) 
    -> void {
    auto itend = EdgeLocks.end();
    for ( auto it = EdgeLocks.begin(); it != itend; ++it) {
      releaseEdgeLock((*it).first, 
          (*it).second.first, (*it).second.second);
    }
  }

  auto releaseVertexAll(LockListType & VertexLocks) 
    -> void {
    auto itend = VertexLocks.end();
    for ( auto it = VertexLocks.begin(); it != itend; ++it) {
      releaseVertexLock((*it).first, 
          (*it).second.first, (*it).second.second);
    }
  }

  auto releaseAll(LockListType & VertexLocks, LockListType & EdgeLocks) 
    -> void {
      releaseVertexAll(VertexLocks);
      releaseEdgeAll(EdgeLocks);
  }

  auto releaseEdgeLock(unsigned int id, MutexType mt, LockType lt) 
    -> bool {
    if (EdgeLockMap.find(id) == EdgeLockMap.end()) {
      exit(0);
    }
    else {
      MutexPointer mp(nullptr);
      switch(mt) {
        case ID:
          mp = EdgeLockMap[id].getIdMutex();
          break;
        case Pp:
          mp = EdgeLockMap[id].getPpMutex();
          break;
        case FV:
          mp = EdgeLockMap[id].getFVMutex();
          break;
        case SV:
          mp = EdgeLockMap[id].getSVMutex();
          break;
        case FNE:
          mp = EdgeLockMap[id].getFNEMutex();
          break;
        case FPE:
          mp = EdgeLockMap[id].getFPEMutex();
          break;
        case SNE:
          mp = EdgeLockMap[id].getSNEMutex();
          break;
        case SPE:
          mp = EdgeLockMap[id].getSPEMutex();
          break;
        default:
          std::cerr << "ERROR: No such Mutex in EdgeLock\n";
          exit(0);
      }//END_SWITCH

      switch (lt) {   
        case SH:
          mp->unlock_shared();
          break;
        case EX:
          mp->unlock(); 
          break;
        default:
          std::cerr << "ERROR: No such Mutex in EdgeLock\n";
          exit(0);
      }
      return true;
    }
  }

    /// TODO check map
  auto addToVertexLockMap(unsigned int id) 
    -> void  {
      VertexLock NewVertex;
      VertexLockMap.insert(VLockPair(id, NewVertex));
  }

  auto addToEdgeLockMap(unsigned int id) 
    -> void  {
      EdgeLock NewEdge;
      EdgeLockMap.insert(ELockPair(id, NewEdge)); 
  }
 
  auto buildLockMap(GraphType & graph) 
  -> void {
    typedef GraphType::VertexPointer VertexPointer;
    typedef GraphType::EdgePointer EdgePointer;
    std::map<unsigned int, VertexPointer> VertexMap;
    std::map<unsigned int, EdgePointer> EdgeMap;
    VertexMap = graph.getVertexMap();
    EdgeMap = graph.getEdgeMap();

    for(auto iter = VertexMap.begin();
        iter != VertexMap.end(); iter++) {
//      VertexLock NewVertex;
      VertexLock* NewVertexLock = new VertexLock();
      VertexLockMap.insert(VLockPair((*iter).first, *NewVertexLock));
      (*iter).second->setVertexLock(NewVertexLock); 
    }

    for(auto it = EdgeMap.begin();
        it != EdgeMap.end(); it++) {
//      EdgeLock NewEdge;
      EdgeLock* NewEdgeLock = new EdgeLock();
      EdgeLockMap.insert(ELockPair((*it).first, *NewEdgeLock));
      (*it).second->setEdgeLock(NewEdgeLock);
    }

//    cout << "after build maps, vertex lock num " << VertexLockMap.size() << " edge lock num " << EdgeLockMap.size() << endl;
  }
 

  auto getVertexLockMap() 
    -> VLockMapType {
      return VertexLockMap;
  }
  auto getEdgeLockMap() 
    -> ELockMapType {
      return EdgeLockMap;
  }

#else
  ///locks are encoded in Vertex and Edge
public:
  typedef GraphType::VertexPointer VertexPtr;
  typedef GraphType::EdgePointer EdgePtr;
public:
  LocksManager(GraphType & g) : graph(g) {};

  auto getVertexLock(unsigned int id, MutexType mt, LockType lt) 
    -> bool {
    VertexPtr vp = graph.getVertexPointer(id);
    if(vp == nullptr) {
      std::cerr  << "Error : No such vertex " << id <<" in map \n";
      ///TODO: shoould be exception here, need to be fixed.
      /**
      *exiting process would give out ownership of this mutex automatically,
      * no need to release it before exiting
      * but consistency state?
      * anyway, it won't happen if we don't delete any lock from manager
      */
      exit(0);
    }
    MutexPointer mp(nullptr);
    switch(mt) {
      case Pp:
        mp = vp->getVertexLock()->getPpMutex();
        break;
      case LE:
        mp = vp->getVertexLock()->getLEMutex();
        break;
      case NE:
        mp = vp->getVertexLock()->getNEMutex();
        break;
      case ID:
        mp = vp->getVertexLock()->getIdMutex();
        break;
      default:
        ///TODO should be exception here
        std::cerr << "ERROR: No such Mutex in VertexLock\n";
        exit(0);
    }//END_SWITCH
    switch (lt) {   
      ///Shared lock
      case SH:
        return mp->try_lock_shared();
      case EX:
        return mp->try_lock(); 
      default:
        ///TODO Should be exception here
        std::cerr << "ERROR: No such Mutex in VertexLock\n";
        exit(0);
    }
  }

  auto releaseVertexLock(unsigned int id, MutexType mt, LockType lt) 
    -> bool {
    VertexPtr vp = graph.getVertexPointer(id);
    if(vp == nullptr) {
      std::cerr << "Error : No such vertex " << id <<" in map \n";
      exit(0);
    }
    MutexPointer mp(nullptr);
    switch(mt) {
      case Pp:
        mp = vp->getVertexLock()->getPpMutex();
        break;
      case LE:
        mp = vp->getVertexLock()->getLEMutex();
        break;
      case NE:
        mp = vp->getVertexLock()->getNEMutex();
        break;
      case ID:
        mp = vp->getVertexLock()->getIdMutex();
        break;
      default:
        ///TODO should be exception here
        std::cerr << "ERROR: No such Mutex in VertexLock\n";
        exit(0);
    }//END_SWITCH

    switch (lt) {   
      ///Shared lock
      case SH:
        mp->unlock_shared();
        break;
      case EX:
        mp->unlock(); 
        break;
      default:
        ///TODO Should be exception here
        std::cerr << "ERROR: No such Mutex in VertexLock\n";
        exit(0);
    }
    return true;
  }

  auto getEdgeLock(unsigned int id, MutexType mt, LockType lt) 
    -> bool {
    EdgePtr ep = graph.getEdgePointer(id);
    if(ep == nullptr) {
      std::cerr << "Error : No such edge" << id <<" in map \n";
      exit(0);
    }
    MutexPointer mp(nullptr);
    switch(mt) {
      case ID:
        mp = ep->getEdgeLock()->getIdMutex();
        break;
      case Pp:
        mp = ep->getEdgeLock()->getPpMutex();
        break;
      case FV:
        mp = ep->getEdgeLock()->getFVMutex();
        break;
      case SV:
        mp = ep->getEdgeLock()->getSVMutex();
        break;
      case FNE:
        mp = ep->getEdgeLock()->getFNEMutex();
        break;
      case FPE:
        mp = ep->getEdgeLock()->getFPEMutex();
        break;
      case SNE:
        mp = ep->getEdgeLock()->getSNEMutex();
        break;
      case SPE:
        mp = ep->getEdgeLock()->getSPEMutex();
        break;
      default:
        std::cerr << "ERROR: No such Mutex in EdgeLock\n";
        exit(0);
    }//END_SWITCH
    switch (lt) {   
      case SH:
        return mp->try_lock_shared();
      case EX:
        return mp->try_lock(); 
      default:
        std::cerr  << "ERROR: No such Mutex in EdgeLock\n";
        exit(0);
    }
  }

  auto releaseEdgeLock(unsigned int id, MutexType mt, LockType lt) 
    -> bool {
    EdgePtr ep = graph.getEdgePointer(id);
    if (ep == nullptr) {
      exit(0);
    }
    MutexPointer mp(nullptr);
    switch(mt) {
      case ID:
        mp = ep->getEdgeLock()->getIdMutex();
        break;
      case Pp:
        mp = ep->getEdgeLock()->getPpMutex();
        break;
      case FV:
        mp = ep->getEdgeLock()->getFVMutex();
        break;
      case SV:
        mp = ep->getEdgeLock()->getSVMutex();
        break;
      case FNE:
        mp = ep->getEdgeLock()->getFNEMutex();
        break;
      case FPE:
        mp = ep->getEdgeLock()->getFPEMutex();
        break;
      case SNE:
        mp = ep->getEdgeLock()->getSNEMutex();
        break;
      case SPE:
        mp = ep->getEdgeLock()->getSPEMutex();
        break;
      default:
        std::cerr << "ERROR: No such Mutex in EdgeLock\n";
        exit(0);
    }//END_SWITCH

    switch (lt) {   
      case SH:
        mp->unlock_shared();
        break;
      case EX:
        mp->unlock(); 
        break;
      default:
        std::cerr << "ERROR: No such Mutex in EdgeLock\n";
        exit(0);
    }
    return true;
  }

  auto releaseEdgeAll(LockListType & EdgeLocks) 
    -> void {
    auto itend = EdgeLocks.end();
    for ( auto it = EdgeLocks.begin(); it != itend; ++it) {
      releaseEdgeLock((*it).first, 
          (*it).second.first, (*it).second.second);
    }
  }

  auto releaseVertexAll(LockListType & VertexLocks) 
    -> void {
    auto itend = VertexLocks.end();
    for ( auto it = VertexLocks.begin(); it != itend; ++it) {
      releaseVertexLock((*it).first, 
          (*it).second.first, (*it).second.second);
    }
  }

  auto releaseAll(LockListType & VertexLocks, LockListType & EdgeLocks) 
    -> void {
      releaseVertexAll(VertexLocks);
      releaseEdgeAll(EdgeLocks);
  }


  auto addToVertexLockMap(unsigned int id) 
    -> void  {
      VertexPtr vp = graph.getVertexPointer(id);
      if ( vp == nullptr) {
        std::cerr  << "Error : No such vertex " << id <<" in map \n";
        exit(0);
      }

      VertexLock* NewVertexLock = new VertexLock();
      vp->setVertexLock(NewVertexLock);
    }

  auto addToEdgeLockMap(unsigned int id) 
    -> void  {
      EdgePtr ep = graph.getEdgePointer(id);
      if ( ep == nullptr) {
        std::cerr  << "Error : No such edge" << id <<" in map \n";
        exit(0);
      }

      EdgeLock*  NewEdgeLock = new EdgeLock();
      ep->setEdgeLock(NewEdgeLock);
  }
 
  auto buildLockMap(GraphType & graph) 
  -> void {
    typedef GraphType::VertexPointer VertexPointer;
    typedef GraphType::EdgePointer EdgePointer;
    std::map<unsigned int, VertexPointer> VertexMap;
    std::map<unsigned int, EdgePointer> EdgeMap;
    VertexMap = graph.getVertexMap();
    EdgeMap = graph.getEdgeMap();

    for(auto iter = VertexMap.begin();
        iter != VertexMap.end(); iter++) {
      VertexLock* NewVertexLock = new VertexLock();
      (*iter).second->setVertexLock(NewVertexLock); 
    }

    for(auto it = EdgeMap.begin();
        it != EdgeMap.end(); it++) {
      EdgeLock* NewEdgeLock = new EdgeLock();
      (*it).second->setEdgeLock(NewEdgeLock);
    }
  }
#endif

protected:
#ifndef _LOCKING_
  VLockMapType VertexLockMap;
  ELockMapType EdgeLockMap;
#else 
  GraphType & graph;
#endif
    
};

#endif /*_LOCKSMANAGER_H_*/
