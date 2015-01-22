#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <vector>
#include <iostream>
#include <string>

#include "PropertyList.h"

class Edge;

class Vertex { 
public:
  typedef Vertex* VertexPtr;
  typedef Edge* EdgePtr;
  typedef unsigned int EdgeDescriptor;
  typedef std::string KeyType;
  typedef std::string ValueType;

  // PropertyList typedefs.
  typedef PropertyList< KeyType, ValueType > PropertyListType;
  typedef pair<ValueType, bool> ReturnPropertyValueType;

public:
  Vertex();
  void addEdge(EdgePtr ep);
  void dump();
  std::vector<EdgePtr> & getEdges();
  unsigned int getId();
  EdgePtr getNextEdge();
  ReturnPropertyValueType getPropertyValue(KeyType k);
  void setId(unsigned int id);
  void setNextEdge(EdgePtr e);
  void setProperty(KeyType k, ValueType v);
  void setPropertyList(PropertyListType l);
  PropertyListType &  getPropertyList();

protected:
  // Vertex id.
  unsigned int _id;
  // Pointer to next relationship
  EdgePtr _nextEdge;

  std::vector<EdgePtr> _outEdges;

  // TODO: pointer to property
  PropertyListType _list;
};

#endif /* _VERTEX_H_ */