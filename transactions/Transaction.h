//===-- transactions/Transaction.h - Log class --*- C++ -*-===//
//
//                     CAESR Graph Database 
//
// TODO: LICENSE
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This is the main class for managing log records for transactions
///
//===----------------------------------------------------------------------===//

#ifndef _TRANSACTION_H_
#define _TRANSACTION_H_

#include "Concurrency_control_config.h"
#include "Lock.h"
#include "GraphType.h"
//#include "LocksManager.h"
#include "global.h"

#include <stdlib.h>
#include <unordered_map>

//#define _TRANX_STATS_ true

class LocksManager;

class Transaction {
public:
  typedef unsigned int IdType;
  typedef GraphType::VertexPointer VertexPointer;
  typedef GraphType::EdgePointer EdgePointer;
  typedef VertexLock::MutexPointer MutexPointer;
  typedef std::pair<VertexPointer, LockType> VLockPairType;
  typedef std::pair<EdgePointer, LockType> ELockPairType;
  typedef std::pair<MutexPointer, VLockPairType> VertexLockPairType; 
  typedef std::unordered_map<MutexPointer, VLockPairType> VertexLockMapType; 
  typedef std::pair<MutexPointer, ELockPairType> EdgeLockPairType;
  typedef std::unordered_map<MutexPointer, ELockPairType> EdgeLockMapType;
#ifdef _TRANX_STATS_
  typedef std::pair<MutexPointer, int> MutexPairType;
  typedef std::unordered_map<MutexPointer, int> MutexMapType;
#endif
public:
  Transaction(); 

  Transaction(IdType id); 

//  Transaction(IdType id, LocksManager & LockManager); 

  IdType getId();

  bool begin();

  bool expand();

  bool commit();

  bool abort(); 

  bool abort(VertexPointer, MutexType, LockType); 

  bool abort(EdgePointer, MutexType, LockType); 

  bool rollBack();

  bool close(); 

  void releaseVertexLock();

  void releaseEdgeLock();

  void releaseLock();

  /*TODO Prototype to get/release locks*/
  bool checkVertexLock(MutexPointer, LockType);
  bool checkEdgeLock(MutexPointer, LockType);

  bool getVertexLock(VertexPointer, MutexType, LockType);
  bool getEdgeLock(EdgePointer, MutexType, LockType);

  bool releaseVertexLock(VertexPointer, MutexType, LockType);
  bool releaseEdgeLock(EdgePointer, MutexType, LockType);

  bool waitOn(VertexPointer, MutexType, LockType);
  bool waitOn(EdgePointer, MutexType, LockType);
  /*TODO end**/

  bool registerVertexLock(VertexPointer, MutexPointer , LockType );
  bool registerEdgeLock(EdgePointer, MutexPointer , LockType );

  VertexLockMapType getVertexLockMap();

  EdgeLockMapType getEdgeLockMap();

  TransStatusType checkStatus();

  ~Transaction();

  friend class LocksManager;
//  friend bool LocksManager::detectDeadlock();
#ifdef _TRANX_STATS_
  void visitMutex(MutexPointer MutexPtr);
  void abortMutex(MutexPointer MutexPtr);
  int getAbortNum();
  MutexMapType getAbortedMap();
  MutexMapType getVisitedMap();
  void dumpAbortNum();
  void dumpAbortedMap();
  void dumpVisitedMap();
  void dumpStats();
  uint64_t getBeginTime();
  uint64_t getCommitTime();
  uint64_t getExpandTime();
  void setBeginTime();
  void setCommitTime();
  void setExpandTime();
  void setCloseTime();
  uint64_t setTime();
  uint64_t getActiveTime();
  uint64_t getExecTime();
  uint64_t getShrinkTime();
  uint64_t getProcTime();
#endif

protected:
//  LocksManager * LockManager;
  IdType  TransId;
  TransStatusType TransStatus;
  EdgeLockMapType EdgeLockMap;
  VertexLockMapType VertexLockMap;
#ifdef _TRANX_STATS_
  int NumAbort;
  MutexMapType AbortedMap;
  MutexMapType VisitedMap;
  void initStats();
  uint64_t BeginTime;
  uint64_t CommitTime;
  uint64_t ExpandTime;
  uint64_t CloseTime;
#endif 
};

#endif /*_TRANSACTION_H_*/
