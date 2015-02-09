#ifndef _BASE_GRAPH_H_
#define _BASE_GRAPH_H_

#include <vector>
#include <new>
#include <stdlib.h>

#include "Vertex.h"
#include "Edge.h"
#include "PropertyList.h"

using namespace std;

class GraphType {
public:
  typedef unsigned int VertexDescriptor;
  typedef unsigned int EdgeDescriptor;
  typedef Vertex* VertexPointer;
  typedef Vertex* VertexPtr;
  typedef Edge* EdgePointer;
  typedef Edge* EdgePtr;
  typedef PropertyList<FixedString, FixedString> PropertyListType;
  typedef PropertyListType VertexPropertyList;
  typedef PropertyListType EdgePropertyList;

public:
  VertexPointer getVertexPointer(VertexDescriptor Vertex) {
    if ( (Vertex > Vertices.size() - 1) || (Vertex < 0) ) {
      return nullptr;
    }
    return Vertices[Vertex];
  }

  VertexDescriptor addVertex() {
    VertexPointer NewVertex = new Vertex();

    NewVertex->setId(NumberOfVertices); 
    ++NumberOfVertices;
    Vertices.push_back(NewVertex);
    return NewVertex->getId();
  }

  VertexDescriptor addVertex(PropertyListType & InitialPropertyList) {
    VertexPointer NewVertex = new Vertex();
    NewVertex->setPropertyList(InitialPropertyList);
    NewVertex->setId(NumberOfVertices); 
    ++NumberOfVertices;
    Vertices.push_back(NewVertex);
    return NewVertex->getId();
  }

  void chainEdges(VertexPointer Vertex, EdgePointer FirstNextEdge, EdgePointer NewEdge) {
    EdgePointer NextEdge = FirstNextEdge;
    EdgePointer PreviousEdge = nullptr;

    while ( NextEdge != nullptr ) {
      PreviousEdge = NextEdge;
      if ( NextEdge->getFirstVertexPtr()->getId() == Vertex->getId() ) {
	NextEdge = NextEdge->getFirstNextEdge();
      } else if ( NextEdge->getSecondVertexPtr()->getId() == Vertex->getId() ) {
	NextEdge = NextEdge->getSecondNextEdge();
      } else {
        std::cout << "+ \nERROR: no forward movement \n";
	//	cout << "+ Vertex focus: " << v->getId() << endl;
	//	cout << "+ next: End of chain edge" << endl;
	//	NextEdge->dump();
	//	cout << "+ NewEdge: New edge to insert" << endl;
	//	NewEdge->dump();
      }
    }

    //    cout << "end of chain edge: " << PreviousEdge->getId() << endl;
    if ( PreviousEdge == NewEdge )  {
      //      cout << "FIRST EDGE: Not hooking up " << PreviousEdge->getId() << " to " << NewEdge->getId() << endl;
      return;
    }
    
    // Got to the end of the chain.
    if ( PreviousEdge->getFirstVertexPtr() == Vertex ) {
      ///      cout << "hooking up " << PreviousEdge->getId() << " to " << NewEdge->getId() << endl;
      PreviousEdge->setFirstNextEdge(NewEdge);
      if ( NewEdge->getFirstVertexPtr() == Vertex ) {
	NewEdge->setFirstPreviousEdge(PreviousEdge);
	NewEdge->setFirstNextEdge(nullptr); 
      } else if (NewEdge->getSecondVertexPtr() == Vertex ) {
	NewEdge->setSecondPreviousEdge(PreviousEdge);
	NewEdge->setSecondNextEdge(nullptr); 
      }

    } else if ( PreviousEdge->getSecondVertexPtr() == Vertex ) {
      //      cout << "hooking up " << PreviousEdge->getId() << " to " << NewEdge->getId() << endl;
      PreviousEdge->setSecondNextEdge(NewEdge);
      if ( NewEdge->getFirstVertexPtr() == Vertex ) {
	NewEdge->setFirstPreviousEdge(PreviousEdge);
	NewEdge->setFirstNextEdge(nullptr);

      } else if ( NewEdge->getSecondVertexPtr() == Vertex ) {
        //	cout << "hooking up " << PreviousEdge->getId() << " to " << NewEdge->getId() << endl;
	NewEdge->setSecondPreviousEdge(PreviousEdge);
	NewEdge->setSecondNextEdge(nullptr);
      }
    }
  }

  void assignPointers(VertexDescriptor vs, VertexDescriptor vd, 
                      EdgePointer NewEdge) {
    
    VertexPointer FirstVertexPointer = NewEdge->getFirstVertexPtr();
    VertexPointer SecondVertexPointer = NewEdge->getSecondVertexPtr();

    /// 1. See if first's and second's nextEdge is set or not.
    /// If it is not set then set it. Doesn't matter who the next really is.
    if ( FirstVertexPointer->getNextEdge() == nullptr ) {
      FirstVertexPointer->setNextEdge(NewEdge);
    }
    if ( SecondVertexPointer->getNextEdge() == nullptr ) {
      SecondVertexPointer->setNextEdge(NewEdge);
    }

    /// 2. Find the end of the chain for each first/second node.
    /// The chain is going to iterate over first and second pointers based on who is source.
    EdgePointer FirstNextEdge = FirstVertexPointer->getNextEdge();
    chainEdges(FirstVertexPointer, FirstNextEdge, NewEdge);
    /// Chain the edges for the second vertex.
    FirstNextEdge = SecondVertexPointer->getNextEdge();
    chainEdges(SecondVertexPointer, FirstNextEdge, NewEdge);

  }

  EdgeDescriptor addEdge(VertexDescriptor StartVertex, VertexDescriptor EndVertex) {
    EdgePointer NewEdge = new Edge(Vertices[StartVertex], Vertices[EndVertex]);
    NewEdge->setId(NumberOfEdges);
    assignPointers(StartVertex, EndVertex, NewEdge);
    ++NumberOfEdges;
    Edges.push_back(NewEdge);
    return NewEdge->getId();
  }

  EdgeDescriptor addEdge(VertexDescriptor StartVertex, 
                         VertexDescriptor EndVertex, const string & Label) {
    EdgePointer NewEdge = new Edge(Vertices[StartVertex], Vertices[EndVertex]);
    
    NewEdge->setId(NumberOfEdges);
    NewEdge->setType(Label);
    assignPointers(StartVertex, EndVertex, NewEdge);
    ++NumberOfEdges;
    Edges.push_back(NewEdge);
    return NewEdge->getId();
  }

  EdgeDescriptor addEdge(VertexDescriptor StartVertex, 
                         VertexDescriptor EndVertex, 
                         PropertyListType & InitialPropertyList) {

    EdgePointer NewEdge = new Edge(Vertices[StartVertex], Vertices[EndVertex]);

    NewEdge->setPropertyList(InitialPropertyList);
    NewEdge->setId(NumberOfEdges);    
    assignPointers(StartVertex, EndVertex, NewEdge);
    ++NumberOfEdges;
    Edges.push_back(NewEdge);
    return NewEdge->getId();
  }

  EdgeDescriptor addEdge(VertexDescriptor StartVertex, VertexDescriptor EndVertex, 
                         const string & Label, 
                         PropertyListType & InitialPropertyList) {

    EdgePointer NewEdge = new Edge(Vertices[StartVertex], Vertices[EndVertex]);

    NewEdge->setType(Label);
    NewEdge->setPropertyList(InitialPropertyList);
    NewEdge->setId(NumberOfEdges);    
    assignPointers(StartVertex, EndVertex, NewEdge);
    ++NumberOfEdges;
    Edges.push_back(NewEdge);
    return NewEdge->getId();
  }

  void dump() {
    for (auto i = 0; i < Vertices.size(); i++) {
      Vertices[i]->dump();
    }

    for (auto i =0; i <Edges.size() ; i++) {
      Edges[i]->dump();
      std::cout << endl;
    }
  }

 GraphType(): NumberOfVertices(0), NumberOfEdges(0) {

  }

  ~GraphType() {
    // Must manually delete the objects.  
    // However, only one place is necessary since everywhere else, I am storing pointers.
    // Thus, Vertices and _edges contain all newly created objects.
//    cout << "Delete everything\n";
    for (auto i =0; i < Vertices.size(); i++) {
      Vertices[i]->deleteVertex();
      delete Vertices[i];
    }

    for (auto i =0; i < Edges.size(); i++) {
      Edges[i]->deleteEdge();
      delete Edges[i];
    }

    // Delete the memory spaces.
    //    delete _nodeMemory;
    //cout << "Delete edge  memory\n";
    //    delete _edgeMemory;
  }

  void allocVertexMemory(unsigned int sz) {

    // Allocation sz number of Vertex objects.
    /* _nodeMemory = new char[sizeof(Vertex)*sz]; */
    /* cout << "Vertex Memory\n + Starting address: " << reinterpret_cast<int*>(_nodeMemory)  */
    /* 	 << ", ending address: " << reinterpret_cast<int*>(_nodeMemory + sizeof(Vertex)*sz) << "\n"; */
  }

  void allocEdgeMemory(unsigned int sz) {
    // Allocation sz number of Vertex objects.
    //cout << "Edge space: " << sizeof(Edge)*sz << "\n";
    //    _edgeMemory = new char[sizeof(Edge)*sz];
    //    cout << "Edge Memory\n + Starting address: " << reinterpret_cast<int*>(_edgeMemory) 
    //	 << ", ending address: " << reinterpret_cast<int*>(_edgeMemory + sizeof(Edge)*sz) << "\n" << endl;
  }

protected:
  vector<VertexPointer> Vertices;
  vector<EdgePointer> Edges;
  unsigned int NumberOfVertices;
  unsigned int NumberOfEdges;
  //  char* _nodeMemory;
  //  char* _edgeMemory;

};

#endif /* _BASE_GRAPH_H */

