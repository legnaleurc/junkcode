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
using DriveNodeCSP = std::shared_ptr<const DriveNode>;
using DriveNodeWP = std::weak_ptr<DriveNode>;
using DriveNodeCWP = std::weak_ptr<const DriveNode>;


class DriveNode {
public:
    DriveNode(const DriveFileInfo & info, DriveNodeSP parent);
    DriveNode() = delete;
    DriveNode(const DriveNode &) = delete;
    DriveNode & operator ()(const DriveNode &) = delete;

    // children shouldn't normally be accessed directly, use node()
    inline int visibleLocation(const QString &childName) {
        return visibleChildren.indexOf(childName);
    }

    void populate(const DriveFileInfo &fileInfo) {
        this->info = fileInfo;
    }

    inline bool operator <(const DriveNode &node) const {
        return this->info.fileName() < node.info.fileName();
    }
    inline bool operator >(const QString &name) const {
        return this->info.fileName() > name;
    }
    inline bool operator <(const QString &name) const {
        return this->info.fileName() < name;
    }
    bool operator ==(const QString &name) const {
        return this->info.fileName() == name;
    }

    inline bool hasInformation() const {
        return this->info.isValid();
    }

    // basic
    DriveFileInfo info;
    DriveNodeWP parent;
    std::unordered_map<QString, DriveNodeSP> children;
    // view
    bool populatedChildren;
    bool isVisible;
    QList<QString> visibleChildren;
    int dirtyChildrenIndex;
};

#endif
