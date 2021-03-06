#include "exception.hpp"
#include "output.hpp"
#include "type.hpp"

namespace libblock {

void Instance::typeCheck(Type &type1, Type &type2) {
    if (type1 != type2) {
        throw ErrorTypeCollision {};
    }
}

Instance::Instance():
    header {*new (GC) OutputTask},
    content {*new (GC) OutputTask} {}

std::string Instance::strFunc() const {
    return "func_" + std::to_string(tuid);
}

std::string Instance::strStruct() const {
    return "struct struct_" + std::to_string(tuid);
}

std::string Instance::strSelf() const {
    return "((" + strStruct() + " *) self)";
}

std::string Instance::strLabel(size_t position) const {
    return "label_"
        + std::to_string(tuid) + "_"
        + std::to_string(position);
}

std::string Instance::strSpecialLabel(
    size_t position, std::string &&name
) const {
    return name + "_"
        + std::to_string(tuid) + "_"
        + std::to_string(position);
}

std::string Instance::strCalleeType(size_t position) const {
    return callee_types.at(position).get().strStruct();
}

std::string Instance::strCalleeName(size_t position) const {
    return "object_"
        + std::to_string(tuid) + "_"
        + std::to_string(position);
}

std::string Instance::strInner(size_t position) const {
    return "((" + strCalleeType(position) + " *) inner)";
}

std::string Instance::strCallee(size_t position) const {
    return "((" + strCalleeType(position) + " *) callee)";
}

bool Instance::in(Instance &instance) const {
    for (const auto &symbol: symbol_types) {
        if (symbol.second != instance.at(symbol.first)) {
            return false;
        }
    }

    return true;
}

Type &Instance::at(const std::string &name) {
    if (name == "self") {
        return *this;
    } else {
        const auto &symbol = symbol_types.find(name);

        if (symbol != symbol_types.end()) {
            return symbol->second;
        } else {
            throw ErrorSymbolNotFound {};
        }
    }
}

void Instance::insert(const std::string &name, Type &type) {
    if (name == "self") {
        typeCheck(*this, type);
    } else {
        const auto &symbol = symbol_types.find(name);

        if (symbol != symbol_types.end()) {
            typeCheck(symbol->second, type);
        } else {
            if (locked) {
                throw ErrorSymbolNotFound {};
            } else {
                symbols.push_back(name);
                symbol_types.insert({name, type});
            }
        }
    }
}

Type &Instance::lookup(
    const std::string &name,
    size_t &level
) {
    if (name == "self") {
        return *this;
    } else {
        const auto &symbol = symbol_types.find(name);

        if (symbol != symbol_types.end()) {
            return symbol->second;
        } else {
            ++level;

            return at("parent").prepareLookup().lookup(name, level);
        }
    }
}

void Instance::lookupCheck(
    const std::string &name, Type &type,
    size_t &level
) {
    if (name == "self") {
        typeCheck(*this, type);
    } else {
        const auto &symbol = symbol_types.find(name);

        if (symbol != symbol_types.end()) {
            typeCheck(symbol->second, type);
        } else {
            ++level;

            at("parent").prepareLookup().lookupCheck(name, type, level);
        }
    }
}

size_t Instance::addPosition() {
    return last_position++;
}

void Instance::addCallee(size_t position, Instance &callee) {
    callee_types.insert({position, callee});
}

void Instance::lock() {
    locked = true;
}

void Instance::renderStruct(OutputContext &och) const {
    och.endl();
    och.os << strStruct() << " {";
    och.enter();

        och.endl();
        och.os << "struct frame frame;";
        och.endl();

        och.endl();
        och.os << "struct {";
        och.enter();

            for (const std::string &symbol: symbols) {
                och.endl();
                och.os << symbol_types.at(symbol).get().strDecl(symbol) << ";";
            }

        och.leave();
        och.endl();
        och.os << "} data;";

    och.leave();
    och.endl();
    och.os << "};";
    och.endl();
}

void Instance::renderFuncHead(OutputContext &oc) const {
    oc.endl();
    oc.os << "LB_ENTER(" << strFunc() << ")";
    oc.enter();
}

void Instance::renderFuncTail(OutputContext &oc) const {
        oc.endl();
        oc.os << "self->func = &func_illegal;";
        oc.endl();
        oc.os << "self = self->caller;";

    oc.leave();
    oc.endl();
    oc.os << "LB_EXIT()";
    oc.endl();
}

std::string Instance::strDecl(const std::string &name) const {
    return strStruct() + " *" + name;
}

std::string Instance::strReint() const {
    return "(*(" + strStruct() + " **) &tmp)";
}

}
