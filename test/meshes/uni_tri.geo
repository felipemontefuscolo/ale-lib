// Gmsh project created on Mon Jul 25 14:37:22 2011
a = 0.05;
Point(1) = {0, 0, 0, a};
Point(2) = {1, 0, 0, a};
Point(3) = {0, 1, 0, a};
Line(1) = {2, 1};
Line(2) = {1, 3};
Line(3) = {3, 2};
Line Loop(4) = {3, 1, 2};
Plane Surface(5) = {4};
