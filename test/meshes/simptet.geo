cl1 = 1;
Point(1) = {0, 0, 0, cl1};
Point(2) = {1, 0, 0, cl1};
Point(3) = {0, 1, 0, cl1};
Point(4) = {0, 0, 1, cl1};
Point(5) = {0, 0, -1, cl1};
Line(1) = {4, 2};
Line(2) = {2, 3};
Line(3) = {3, 1};
Line(4) = {1, 4};
Line(5) = {4, 3};
Line(6) = {1, 2};
Line(7) = {2, 5};
Line(8) = {5, 3};
Line(9) = {5, 1};
Line Loop(11) = {2, 3, 6};
Plane Surface(11) = {11};
Line Loop(13) = {4, 1, -6};
Plane Surface(13) = {13};
Line Loop(15) = {4, 5, 3};
Plane Surface(15) = {15};
Line Loop(17) = {1, 2, -5};
Plane Surface(17) = {17};
Line Loop(19) = {3, -9, 8};
Plane Surface(19) = {19};
Line Loop(21) = {6, 7, 9};
Plane Surface(21) = {21};
Line Loop(23) = {2, -8, -7};
Plane Surface(23) = {23};
Surface Loop(25) = {17, 13, 15, 11};
Volume(25) = {25};
Surface Loop(27) = {23, 19, 21, 11};
Volume(27) = {27};

// +z
zpos = 1;

// -z
zneg = 2;

Physical Point(zpos) = {3, 1, 4, 2};
Physical Line(zpos) = {4, 5, 2, 3, 6, 1};
Physical Surface(zpos) = {15, 17, 13, 11};
Physical Volume(zpos) = {25};
Physical Point(zneg) = {5};
Physical Line(zneg) = {9, 8, 7};
Physical Surface(zneg) = {19, 23, 21};
Physical Volume(zneg) = {27};


