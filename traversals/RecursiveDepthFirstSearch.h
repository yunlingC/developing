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

void recursiveDepthFirstSearch(GraphType & Graph,
			       GraphType::VertexDescriptor VertexSourceId,
			       std::map<GraphType::VertexPointer, bool> &
			       VisitedColor) {

  typedef std::pair<GraphType::VertexPointer, bool> ColorMapPair;
  typedef std::map<GraphType::VertexPointer, bool> ColorMap;

  GraphType::VertexPointer CurrentVertex =
    Graph.getVertexPointer(VertexSourceId);
  
  VisitedColor.insert(ColorMapPair(CurrentVertex, true));

  cout << "==> vid: " << CurrentVertex->getId() << endl;
  // Get the outgoing edges for vertex
  std::vector<Edge::EdgePtr> & Edges = CurrentVertex->getOutEdges();

  // Iterate over all the edges.
  for ( auto EdgeIterator = Edges.begin();  EdgeIterator != Edges.end();
       ++EdgeIterator ) {
    GraphType::VertexPointer TargetVertex =
      (*EdgeIterator)->getTarget(CurrentVertex);

    // Get color and check if false.
    auto VisitedVertex = VisitedColor.find(TargetVertex);
    if ( VisitedVertex == VisitedColor.end() ) {
      recursiveDepthFirstSearch(Graph, TargetVertex->getId(), VisitedColor);
    } else {
      VisitedColor.insert(ColorMapPair(TargetVertex, true));
    }
  }

};

void depthFirstSearch(GraphType & Graph,
		      GraphType::VertexDescriptor StartVertex ) {

  //  typedef std::pair<GraphType::VertexPointer, bool> ColorMapPair;
  typedef std::map<GraphType::VertexPointer, bool> ColorMap;
  ColorMap Color;

  recursiveDepthFirstSearch(Graph, StartVertex, Color);
};
#endif /* _RECURSIVE_DEPTH_FIRST_SEARCH_H_ */