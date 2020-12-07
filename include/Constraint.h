/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#pragma once

namespace G3D {

class Constraint {
public:
    enum class TYPE { GENERIC6DOF, HINGE };

    TYPE getType();
    void setType(TYPE type);

private:
    TYPE m_type;
};
} // namespace G3D
