#include "output.hpp"
#include "node.hpp"

void NodeSymbol::renderPath(std::ostream &os) const {
    for (size_t i = 0; i < level; ++i) {
        os << "->parent";
    }

    if (name != "self") {
        os << "->" << name;
    }
}

void NodeSymbol::buildProc(
    Instance &instance,
    Output &output
) {
    // gen type

    instance.lookup(name, level);

    // render

    OutputContext &oc {output.content(instance)};

    oc.endl();
    oc.os << instance.strCast();
    renderPath(oc.os);
    oc.os << ";";
}

Type &NodeSymbol::buildOut(
    Instance &instance,
    Output &output, const std::string &target
) {
    // get type

    Type &type {
        instance.lookup(name, level)
    };

    // render

    OutputContext &oc {output.content(instance)};

    oc.endl();
    oc.os << target << " = " << instance.strCast();
    renderPath(oc.os);
    oc.os << ";";

    // return

    return type;
}

void NodeSymbol::buildIn(
    Instance &instance, Type &type,
    Output &output, const std::string &target
) {
    // set type

    instance.insert(name, type);

    // render

    OutputContext &oc {output.content(instance)};

    oc.endl();
    oc.os << instance.strCast();
    renderPath(oc.os);
    oc.os << " = " << target << ";";
}
