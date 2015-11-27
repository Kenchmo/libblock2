#include "exception.hpp"
#include "output.hpp"
#include "type.hpp"
#include "node.hpp"

void Instance::typeCheck(Type &type1, Type &type2) {
    if (type1 != type2) {
        throw ErrorTypeCollision {};
    }
}

void Instance::renderStruct(OutputContext &oc) const {
    oc.endl();
    oc.os << strStruct() << " {";
    oc.enter();

        oc.endl();
        oc.os << "struct frame frame;";
        oc.endl();

        for (const auto &symbol: symbol_types) {
            oc.endl();
            oc.os << symbol.second.strDecl(symbol.first) << ";";
        }

    oc.leave();
    oc.endl();
    oc.os << "};";
    oc.endl();
}

void Instance::renderFuncHead(OutputContext &oc) const {
    oc.endl();
    oc.os << strFunc() << ":";
    oc.enter();
}

void Instance::renderFuncTail(OutputContext &oc) const {
    oc.endl();
    oc.os << strFuncExit() << ":";
    oc.endl();
    oc.os << "self->func = &&global_func_error;";
    oc.endl();
    oc.os << "goto *self->caller->func;";

    oc.leave();
    oc.endl();
}

Instance::Instance() {}

std::string Instance::strFunc() const {
    return "func_" + std::to_string(tuid());
}

std::string Instance::strFuncExit() const {
    return "func_exit_" + std::to_string(tuid());
}

std::string Instance::strStruct() const {
    return "struct struct_" + std::to_string(tuid());
}

std::string Instance::strSelf() const {
    return "((" + strStruct() + " *) self)";
}

std::string Instance::strLabel(size_t position) const {
    return "label_"
        + std::to_string(tuid()) + "_"
        + std::to_string(position);
}

std::string Instance::strCalleeType(size_t position) const {
    return callee_types.at(position).strStruct();
}

std::string Instance::strCalleeName(size_t position) const {
    return "object_"
        + std::to_string(tuid()) + "_"
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

size_t Instance::addPosition() {
    return ++last_position;
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

void Instance::check(const std::string &name, Type &type) {
    if (name == "self") {
        typeCheck(*this, type);
    } else {
        const auto &symbol = symbol_types.find(name);

        if (symbol != symbol_types.end()) {
            typeCheck(symbol->second, type);
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
            symbol_types.insert({name, type});
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

Type &Instance::addClosure(Type &parent, NodeBlock &blocks) {
    const auto &closure = closure_types.find(std::ref(blocks));

    if (closure != closure_types.end()) {
        return *closure->second;
    } else {
        return *closure_types.insert({
            std::ref(blocks),
            std::make_shared<TypeClosure>(
                parent, blocks
            )
        }).first->second;
    }
}

void Instance::addCallee(size_t position, Instance &callee) {
    callee_types.insert({position, callee});
}

std::string Instance::strDecl(const std::string &name) const {
    return strStruct() + " *" + name;
}

std::string Instance::strReint(const std::string &name) const {
    return "(*(" + strStruct() + " **) &" + name + ")";
}
