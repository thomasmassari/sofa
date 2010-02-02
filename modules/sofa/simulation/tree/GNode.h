/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_SIMULATION_TREE_GNODE_H
#define SOFA_SIMULATION_TREE_GNODE_H

#include <sofa/simulation/tree/tree.h>
#include <sofa/simulation/common/Node.h>



namespace sofa
{

namespace simulation
{
namespace tree
{


/** Define the structure of the scene. Contains (as pointer lists) Component objects and children GNode objects.
*/
class SOFA_SIMULATION_TREE_API GNode : public simulation::Node
{
public:
    SOFA_CLASS(GNode, simulation::Node);
    GNode( const std::string& name="", GNode* parent=NULL  );

    virtual ~GNode();

    //Pure Virtual method from BaseNode
    /// Add a child node
    virtual void addChild(BaseNode* node);

    /// Remove a child node
    virtual void removeChild(BaseNode* node);

    /// Move a node from another node
    virtual void moveChild(BaseNode* obj);

    /// Add an object and return this. Detect the implemented interfaces and add the object to the corresponding lists.
    virtual bool addObject(core::objectmodel::BaseObject* obj) { return simulation::Node::addObject(obj); }

    /// Remove an object
    virtual bool removeObject(core::objectmodel::BaseObject* obj) { return simulation::Node::removeObject(obj); }

    /// Remove the current node from the graph: consists in removing the link to its parent
    virtual void detachFromGraph() ;





    /// Get parent node (or NULL if no hierarchy or for root node)
    core::objectmodel::BaseNode* getParent();

    /// Get parent node (or NULL if no hierarchy or for root node)
    const core::objectmodel::BaseNode* getParent() const;

    /// Test if the given node is a parent of this node.
    bool hasParent(const BaseNode* node) const
    {
        return getParent() == node;
    }

    /// Test if the given context is a parent of this context.
    bool hasParent(const BaseContext* context) const
    {
        if (context == NULL) return getParent() == NULL;
        else return getParent()->getContext() == context;
    }

    /// Test if the given context is an ancestor of this context.
    /// An ancestor is a parent or (recursively) the parent of an ancestor.
    bool hasAncestor(const BaseNode* node) const
    {
        return hasAncestor(node->getContext());
    }

    /// Test if the given context is an ancestor of this context.
    /// An ancestor is a parent or (recursively) the parent of an ancestor.
    bool hasAncestor(const BaseContext* context) const;


    /// Generic object access, given a set of required tags, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir = SearchUp) const;

    /// Generic object access, given a path from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void* getObject(const sofa::core::objectmodel::ClassInfo& class_info, const std::string& path) const;

    /// Generic list of objects access, given a set of required tags, possibly searching up or down from the current context
    ///
    /// Note that the template wrapper method should generally be used to have the correct return type,
    virtual void getObjects(const sofa::core::objectmodel::ClassInfo& class_info, GetObjectsCallBack& container, const sofa::core::objectmodel::TagSet& tags, SearchDirection dir = SearchUp) const;






    /// Called during initialization to corectly propagate the visual context to the children
    virtual void initVisualContext();

    /// Update the whole context values, based on parent and local ContextObjects
    virtual void updateContext();

    /// Update the visual context values, based on parent and local ContextObjects
    virtual void updateVisualContext(VISUAL_FLAG FILTER=ALLFLAGS);

    /// Update the simulation context values(gravity, time...), based on parent and local ContextObjects
    virtual void updateSimulationContext();


    /// Log time spent on an action category, and the concerned object, plus remove the computed time from the parent caller object
    void addTime(ctime_t t, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* parent);
    Node::ctime_t endTime(ctime_t t0, const std::string& s, core::objectmodel::BaseObject* obj, core::objectmodel::BaseObject* parent);

    /// Return the full path name of this node
    std::string getPathName() const;



    // should this be public ?
    Single<GNode> parent;



    static void create(GNode*& obj, xml::Element<core::objectmodel::BaseNode>* arg)
    {
        obj = new GNode();
        obj->parse(arg);
    }

protected:


    virtual void doAddChild(GNode* node);
    void doRemoveChild(GNode* node);


    /// Execute a recursive action starting from this node.
    /// This method bypass the actionScheduler of this node if any.
    void doExecuteVisitor(simulation::Visitor* action);
    // VisitorScheduler can use doExecuteVisitor() method
    friend class simulation::VisitorScheduler;


};

} // namespace tree

} // namespace simulation

} // namespace sofa

#endif
