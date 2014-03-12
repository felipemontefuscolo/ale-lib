// Gmsh project created on Mon Aug 15 12:11:33 2011
Point(1) = {0, 0, 0, 1.0};
Point(2) = {1, 0, 0, 1.0};
Point(3) = {1, 2, 0, 1.0};
Point(4) = {0, 2, 0, 1.0};
Point(5) = {0.5, 3, 0, 1.0};
Line(1) = {1, 4};
Line(2) = {4, 5};
Line(3) = {5, 3};
Line(4) = {3, 2};
Line(5) = {2, 1};
Line Loop(6) = {3, 4, 5, 1, 2};
Plane Surface(7) = {6};
