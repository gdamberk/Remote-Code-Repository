#pragma once
/////////////////////////////////////////////////////////////////
// Graph.h - Demonstrate how to do Graph walks                 //
//                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2014   //
/////////////////////////////////////////////////////////////////
/*
* This demo displays several Graph traversal strategies.
* Any of them will work for Repository searches.
*
*/
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include <iostream>
#include <unordered_map>

namespace GraphProcessing
{
  /////////////////////////////////////////////////////////////////////
  // Node<V> class
  // - represents graph vertices

  template<typename V>
  class Node;

  template<typename V>
  using Sptr = std::shared_ptr < Node<V> >;
  using Key = std::string;

  template<typename V>
  class Node
  {
  public:
    Node(const std::string& name) : name_(name) { }
    V value() const { return value_; }
    void value(V v) { value_ = v; }
    void addChild(Sptr<V> pNode) { children_.push_back(pNode); }
    std::vector<Sptr<V>>& children() { return children_; }
    std::string& name() { return name_; }
    void mark() { visited_ = true; }
    void unmark() { visited_ = false; }
    bool& marked() { return visited_; }
  private:
    V value_;
    std::string name_;
    std::vector<Sptr<V>> children_;
    bool visited_;
  };
