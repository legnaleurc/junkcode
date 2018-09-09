#ifndef DRIVENODE_H
#define DRIVENODE_H

#include "drivefileinfo.h"

#include <unordered_map>


namespace std {

template<>
struct hash<QString> {
    using argument_type = QString;
    using result_type = uint;
    result_type operator ()(const argument_type & s) const noexcept {
        return qHash(s);
    }
};

}


class DriveNode;
using DriveNodeSP = std::shared_ptr<DriveNode>;
using DriveNodeWP = std::weak_ptr<DriveNode>;


class DriveNode {
public:
    DriveNode(const DriveFileInfo & info, DriveNodeSP parent);
    DriveNode() = delete;
    DriveNode(const DriveNode &) = delete;
    DriveNode & operator ()(const DriveNode &) = delete;

    DriveFileInfo info;
    DriveNodeWP parent;
    std::unordered_map<QString, DriveNodeSP> children;
};

#endif
