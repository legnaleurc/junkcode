#include "drivenode.h"


DriveNode::DriveNode(const DriveFileInfo & info, DriveNodeSP parent)
    : info(info)
    , parent(parent)
    , children()
    , populatedChildren(false)
    , isVisible(false)
    , visibleChildren()
    , dirtyChildrenIndex(-1)
{}


DriveNodeSP DriveNode::child(const QString & id) const {
    auto sit = this->children.find(id);
    if (sit == this->children.end()) {
        return nullptr;
    }
    return sit->second;
}
