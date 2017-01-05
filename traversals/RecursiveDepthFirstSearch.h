//===-- traversals/RecursiveDepthFirstSearch.h - DFS search ----*- C++ -*-===//
//
//                     CAESR Graph Database 
//
// TODO: LICENSE
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This is a simple recursive DFS search.
///
//===----------------------------------------------------------------------===//
#ifndef _RECURSIVE_DEPTH_FIRST_SEARCH_H_
#define _RECURSIVE_DEPTH_FIRST_SEARCH_H_

/// Include the GraphType class.
#include "GraphType.h"
#include "Visitor.h"

#define _DB_ 0

#define _STATS_ true

#ifdef _DB_
#include <iostream>
#endif 

void recursiveDFS(GraphType & Graph,
			       GraphType::VertexDescriptor VertexSourceId,
             Visitor & GraphVisitor, 
			       std::map<GraphType::VertexPointer, bool> & VisitedColor) {

  typedef std::pair<GraphType::VertexPointer, bool> ColorMapPair;

  GraphType::VertexPointer CurrentVertex =
    Graph.getVertexPointer(VertexSourceId);
  
  VisitedColor.insert(ColorMapPair(CurrentVertex, true));

#ifdef _STATS_
    GraphVisitor.countVertex(CurrentVertex);
#endif

  bool VertexMatch = GraphVisitor.visitVertex(CurrentVertex);
  if (VertexMatch == true) { 
    return ;
  }

  bool ExitMatch = GraphVisitor.checkVertex(CurrentVertex);
  if (ExitMatch == true) { 
    /// Should be exit here
    exit(0);
  }

  auto NextEdge = CurrentVertex->getNextEdge();
  while (NextEdge != nullptr) {
#ifdef _STATS_
    GraphVisitor.countEdge(NextEdge);
#endif

    GraphType::VertexPointer TargetVertex =
      NextEdge->getTarget(CurrentVertex);

    GraphVisitor.visitEdge(NextEdge);

    bool BranchMatch = GraphVisitor.scheduleBranch(CurrentVertex, NextEdge, TargetVertex);
    bool RevisitFlag = GraphVisitor.discoverVertex(TargetVertex);

    bool TypeMatch = GraphVisitor.scheduleEdge(NextEdge);

    bool DirectionMatch = GraphVisitor.visitDirection(TargetVertex, NextEdge);

    /// Return instead of breaking out of the loop
    /// Once returned, RDFT traverses path from last vertex it visits.
    /// Exiting is not recommanded
    if (BranchMatch == true) {
      return;
    }
    // Get color and check if false.
    if (VisitedColor.find(TargetVertex) == VisitedColor.end() || RevisitFlag) {

      bool TreeMatch = GraphVisitor.scheduleTree(CurrentVertex, NextEdge, TargetVertex);
      if (TreeMatch) {
        return;
      }

      if (TypeMatch && DirectionMatch) {

        recursiveDFS(Graph, TargetVertex->getId(), 
                                GraphVisitor, VisitedColor);

        GraphVisitor.lastVisit(TargetVertex);
      }
    }
    else {
      GraphVisitor.revisitVertex(TargetVertex);
    }

    NextEdge = NextEdge->getNextEdge(CurrentVertex);
  }
};

void recursiveDepthFirstSearch(GraphType & Graph,
		      GraphType::VertexDescriptor StartVertex,
                      Visitor & GraphVisitor ) {

  typedef std::map<GraphType::VertexPointer, bool> ColorMap;
  ColorMap Color;
  
  GraphVisitor.visitStartVertex(Graph.getVertexPointer(StartVertex));
  recursiveDFS(Graph, StartVertex, GraphVisitor, Color);
  GraphVisitor.finishVisit();
};
#endif /* _RECURSIVE_DEPTH_FIRST_SEARCH_H_ */
