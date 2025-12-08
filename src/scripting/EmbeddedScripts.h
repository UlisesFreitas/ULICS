#ifndef EMBEDDED_SCRIPTS_H
#define EMBEDDED_SCRIPTS_H

namespace EmbeddedScripts {

// The content of our main.lua script is embedded here as a C++ raw string literal.
const char* DEMO_CART = R"lua(
-- This script is now empty. The engine will load cartridges
-- from the 'cartridges' directory in future tasks.
function _update() end
function _draw() clear(0) end
)lua";

} // namespace EmbeddedScripts

#endif // EMBEDDED_SCRIPTS_H