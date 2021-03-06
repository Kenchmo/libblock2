#include "builtin.hpp"
#include "node.hpp"
#include "builder_lib.hpp"

namespace {

using namespace libblock;
using namespace libblock::builder;

Builtin __eq__ {"core", "__eq__", {
    libFuncB3R("$result = $a == $b;"),
    libFuncI3R("$result = $a == $b;"),
    libFuncR3R("$result = $a == $b;"),
}};

Builtin __ne__ {"core", "__ne__", {
    libFuncB3R("$result = $a != $b;"),
    libFuncI3R("$result = $a != $b;"),
    libFuncR3R("$result = $a != $b;"),
}};

Builtin __lt__ {"core", "__lt__", {
    libFuncB3R("$result = $a < $b;"),
    libFuncI3R("$result = $a < $b;"),
    libFuncR3R("$result = $a < $b;"),
}};

Builtin __lte__ {"core", "__lte__", {
    libFuncB3R("$result = $a <= $b;"),
    libFuncI3R("$result = $a <= $b;"),
    libFuncR3R("$result = $a <= $b;"),
}};

Builtin __gt__ {"core", "__gt__", {
    libFuncB3R("$result = $a > $b;"),
    libFuncI3R("$result = $a > $b;"),
    libFuncR3R("$result = $a > $b;"),
}};

Builtin __gte__ {"core", "__gte__", {
    libFuncB3R("$result = $a >= $b;"),
    libFuncI3R("$result = $a >= $b;"),
    libFuncR3R("$result = $a >= $b;"),
}};

}
