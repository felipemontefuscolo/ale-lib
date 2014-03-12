// Gmsh project created on Mon Jul 25 14:47:30 2011
a = 0.1;
Point(1) = {0, 0, 0, a};
Point(2) = {1, 0, 0, a};
Point(3) = {0, 1, 0, a};
Point(4) = {0, 0, 1, a};
Line(1) = {1, 4};
Line(2) = {4, 2};
Line(3) = {2, 3};
Line(4) = {3, 1};
Line(5) = {4, 3};
Line(6) = {1, 2};
Line Loop(7) = {3, 4, 6};
Plane Surface(8) = {7};
Line Loop(9) = {4, 1, 5};
Plane Surface(10) = {9};
Line Loop(11) = {1, 2, -6};
Plane Surface(12) = {11};
Line Loop(13) = {5, -3, -2};
Plane Surface(14) = {13};
Surface Loop(15) = {8, 14, 10, 12};
Volume(16) = {15};


