// Gmsh project created on Sat Aug 30 18:08:09 2014


Point(1) = {0, 0, 0, 1.0};
Point(2) = {1, 0, 0, 1.0};
Point(3) = {0, 1, 0, 1.0};
Point(4) = {0, 0, 1, 1.0};
Line(1) = {1, 2};
Line(2) = {1, 4};
Line(3) = {1, 3};
Circle(4) = {2, 1, 3};
Circle(5) = {3, 1, 4};
Circle(6) = {4, 1, 2};
Line Loop(7) = {2, 6, -1};
Plane Surface(8) = {7};
Line Loop(9) = {3, 5, -2};
Plane Surface(10) = {9};
Line Loop(11) = {1, 4, -3};
Plane Surface(12) = {11};
Line Loop(13) = {6, 4, 5};
Ruled Surface(14) = {13};
Surface Loop(15) = {14, 8, 10, 12};
Volume(16) = {15};
Rotate {{0, 1, 0}, {0, 0, 0}, Pi/2} {
  Duplicata { Volume{16}; }
}
Rotate {{0, 1, 0}, {0, 0, 0}, Pi/2} {
  Duplicata { Volume{17}; }
}
Rotate {{0, 1, 0}, {0, 0, 0}, Pi/2} {
  Duplicata { Volume{31}; }
}


Transfinite Line {6, 4, 19, 20, 5, 47, 34, 33, 39, 3, 2, 1, 25} = 2 Using Progression 1;


Coherence;
Physical Volume(51) = {31, 17, 16, 45};


Reverse Surface {50,8,36,22};

