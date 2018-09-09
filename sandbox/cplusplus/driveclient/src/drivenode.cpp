#include "drivenode.h"


DriveNode::DriveNode(const DriveFileInfo & info, DriveNodeSP parent)
    : info(info)
    , parent(parent)
    , children()
{}
