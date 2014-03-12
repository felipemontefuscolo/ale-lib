// Gmsh project created on Fri Aug 31 07:24:21 2012
lc = 0.3;
Point(1) = {0, 0, 0, lc};
Point(2) = {1, 0, 0, lc};
Point(3) = {-1, 0, 0, lc};
Point(4) = {0, 1, 0, lc};
Point(5) = {0, -1, 0, lc};
Circle(1) = {5, 1, 3};
Circle(2) = {3, 1, 4};
Circle(3) = {4, 1, 2};
Circle(4) = {2, 1, 5};
Line Loop(5) = {3, 4, 1, 2};
Plane Surface(6) = {5};

Physical Line(7) = {3, 4, 1, 2};
Physical Surface(8) = {6};
