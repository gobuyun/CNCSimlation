// Copyright  (C)  2007  Ruben Smits <ruben dot smits at mech dot kuleuven dot be>

// Version: 1.0
// Author: Ruben Smits <ruben dot smits at mech dot kuleuven dot be>
// Maintainer: Ruben Smits <ruben dot smits at mech dot kuleuven dot be>
// URL: http://www.orocos.org/kdl

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "kinfam_io.hpp"
#include "frames_io.hpp"
#include <sstream>

namespace KDL {
std::ostream& operator <<(std::ostream& os, const Joint& joint) {
	return os << joint.getName()<<":["<<joint.getTypeName()
              <<", axis: "<<joint.JointAxis() << ", origin"<<joint.JointOrigin()<<"]";
}

std::istream& operator >>(std::istream& is, Joint& joint) {
	return is;
}

std::ostream& operator <<(std::ostream& os, const JntArray& array) {
	os << "[";
	for (unsigned int i = 0; i < array.rows(); i++)
		os << std::setw(KDL_FRAME_WIDTH) << array(i);
	os << "]";
	return os;
}

std::istream& operator >>(std::istream& is, JntArray& array) {
	return is;
}

std::ostream& operator <<(std::ostream& os, const Jacobian& jac) {
	os << "[";
	for (unsigned int i = 0; i < jac.rows(); i++) {
		for (unsigned int j = 0; j < jac.columns(); j++)
			os << std::setw(KDL_FRAME_WIDTH) << jac(i, j);
		os << std::endl;
	}
	os << "]";
	return os;
}

std::istream& operator >>(std::istream& is, Jacobian& jac) {
	return is;
}
//std::ostream& operator <<(std::ostream& os, const JntSpaceInertiaMatrix& jntspaceinertiamatrix) {
//	os << "[";
//	for (unsigned int i = 0; i < jntspaceinertiamatrix.rows(); i++) {
//		for (unsigned int j = 0; j < jntspaceinertiamatrix.columns(); j++)
//			os << std::setw(KDL_FRAME_WIDTH) << jntspaceinertiamatrix(i, j);
//		os << std::endl;
//	}
//	os << "]";
//	return os;
//}

//std::istream& operator >>(std::istream& is, JntSpaceInertiaMatrix& jntspaceinertiamatrix) {
//	return is;
//}


}
