/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#include "Constraint.h"

namespace G3D {
Constraint::TYPE Constraint::getType() { return m_type; }

void Constraint::setType(TYPE type) { m_type = type; }
} // namespace G3D
