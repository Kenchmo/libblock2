#include "exception.hpp"
#include "output.hpp"
#include "node.hpp"

Instance &Block::matchInstance(
    std::unique_ptr<Instance> &&instance_p,
    Output &output
) {
    for (std::unique_ptr<Instance> &exist_p: instances) {
        bool ok {true};

        for (const auto &symbol: instance_p->symbol_types) {
            if (symbol.second != exist_p->at(symbol.first)) {
                ok = false;
                break;
            }
        }

        if (ok) {
            // found

            return *exist_p;
        }
    }

    // not found

    instances.push_back(std::move(instance_p));
    Instance &instance {*instances.back()};

    output.insert(instance);

    // render (before body)

    std::ostream &os {output.osContent(instance)};

    instance.renderFuncHead(os);
    os << " {\n";

    // build

    buildContent(instance, output);

    // render header

    std::ostream &osh {output.osHeader(instance)};

    instance.renderStruct(osh);
    instance.renderFuncDecl(osh);
    osh << "\n";

    // render (after body)

    os << "}\n\n";

    // return

    return instance;
}

void Block::inSpecialArg(
    Instance &, Instance &,
    size_t, std::unique_ptr<Node> &,
    Output &, const std::string &
) {
    throw ErrorTooManyArguments {}; // TODO: va_args?
}

void Block::outSpecialArg(
    Instance &, Instance &,
    size_t, std::unique_ptr<Node> &,
    Output &, const std::string &
) {
    // nothing, by default // TODO: va_args?
}

void Block::inArg(
    Instance &caller, Instance &instance,
    size_t index, std::unique_ptr<Node> &arg,
    Output &output, const std::string &target
) {
    if (
        index >= params.size()
        || params[index].second == SymbolMode::special
    ) {
        inSpecialArg(caller, instance, index, arg, output, target);
    } else if (
        params[index].second == SymbolMode::in
        || params[index].second == SymbolMode::var
    ) {
        instance.insert(
            params[index].first,
            arg->buildOut(
                caller,
                output, target + "->" + params[index].first // TODO: use callback?
            )
        );
    }
}

void Block::outArg(
    Instance &caller, Instance &instance,
    size_t index, std::unique_ptr<Node> &arg,
    Output &output, const std::string &target
) {
    if (
        index >= params.size()
        || params[index].second == SymbolMode::special
    ) {
        outSpecialArg(caller, instance, index, arg, output, target);
    } else if (
        params[index].second == SymbolMode::out
        || params[index].second == SymbolMode::var
    ) {
        arg->buildIn(
            caller,
            instance.at(params[index].first),
            output, target + "->" + params[index].first // TODO: use callback?
        );
    }
}

void Block::build(
    Output &output,
    std::function<void (Instance &)> &&before,
    std::function<void (Instance &)> &&after
) {
    // init

    std::unique_ptr<Instance> instance_p {
        new Instance {}
    };

    // in

    before(*instance_p);

    // find or create instance

    Instance &instance {
        matchInstance(std::move(instance_p), output)
    };

    // out

    after(instance);
}

void Block::buildProc(
    Instance &,
    Output &
) {
    // nothing
}

Type &Block::addClosureType(Instance &instance) {
    if (
        closure_types.find(instance.tuid()) != closure_types.end()
    ) {
        return *closure_types.at(instance.tuid());
    }

    return *closure_types.insert({
        instance.tuid(),
        std::make_shared<TypeClosure>(
            instance, *this
        )
    }).first->second;
}

Type &Block::buildOut(
    Instance &instance,
    Output &output, const std::string &target
) {
    // get type

    Type &type {addClosureType(instance)};

    // render

    std::ostream &os {output.osContent(instance)};

    os << "    " << target << " = self;\n";

    // return

    return type;
}

void Block::buildIn(
    Instance &, Type &,
    Output &, const std::string &
) {
    throw ErrorWriteNotAllowed {};
}
