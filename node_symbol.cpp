#include "output.hpp"
#include "node.hpp"

void NodeSymbol::renderPath(std::ostream &os) const {
    os << "self";

    for (size_t i = 0; i < level; ++i) {
        os << "->parent";
    }

    for (const std::string &i: path) {
        if (i != "self") {
            os << "->" << i;
        }
    }

    if (name != "self") {
        os << "->" << name;
    }
}

void NodeSymbol::buildProc(Instance &instance, Output &output) {
    // gen type

    instance.fullLookup(path, name, level);

    // render

    std::ostream &os {output.osContent(instance)};

    os << "    ";
    renderPath(os);
    os << ";\n";
}

Type &NodeSymbol::buildOut(Instance &instance, Output &output) {
    // get type

    Type &type {instance.fullLookup(path, name, level)};

    // render

    std::ostream &os {output.osContent(instance)};

    os << "    ";
    os << type.decl(nuidOut());
    os << " = ";
    renderPath(os);
    os << ";\n";

    // return

    return type;
}

void NodeSymbol::buildIn(Instance &instance, Type &type, Output &output) {
    // set type

    instance.lookup(path).insert(name, type);

    // render

    std::ostream &os {output.osContent(instance)};

    os << "    ";
    renderPath(os);
    os << " = " << nuidIn() << ";\n";
}
