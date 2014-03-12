// Gmsh project created on Mon Aug 15 14:42:47 2011
Point(1) = {0, 0, 0, 1.0};
Point(2) = {1, 0, 0, 1.0};
Point(3) = {1, 0, 1, 1.0};
Point(4) = {0, 0, 1, 1.0};

Line(1) = {4, 3};
Line(2) = {3, 2};
Line(3) = {2, 1};
Line(4) = {1, 4};
Extrude {0, 1, 0} {
  Line{3, 4, 1, 2};
}
Point(11) = {0.5, 1.5, 0.5, 1.0};
Point(12) = {0.5, 1, 0.5, 1.0};
Line(21) = {12, 10};
Line(22) = {12, 11};
Line(23) = {11, 8};
Line(24) = {8, 12};
Line(25) = {11, 10};
Line Loop(26) = {23, 13, -25};
Plane Surface(27) = {26};
Line Loop(28) = {13, -21, -24};
Plane Surface(29) = {28};
Line Loop(30) = {23, 24, 22};
Plane Surface(31) = -{30};
Line Loop(32) = {21, -25, -22};
Plane Surface(33) = -{32};
Line Loop(34) = {17, 5, 9, 24, 21};
Plane Surface(35) = {34};
Line Loop(36) = {2, 3, 4, 1};
Plane Surface(37) = -{36};
Surface Loop(38) = {35, 20, 37, 8, 12, 16, 29};
Volume(39) = {38};
Surface Loop(40) = {27, 31, 33, 29};
Volume(41) = {40};

