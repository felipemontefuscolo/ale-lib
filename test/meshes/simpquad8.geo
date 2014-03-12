// Gmsh project created on Sat Jul 16 22:03:19 2011
Point(1) = {0, 0, 0, 1.0};
Point(2) = {0, 1, 0, 1.0};
Point(3) = {1, 1, 0, 1.0};
Point(4) = {1, 0, 0, 1.0};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
Line Loop(5) = {3, 4, 1, 2};
Plane Surface(6) = {5};
Transfinite Line {1, 4, 3, 2} = 4 Using Progression 1;
Transfinite Surface {6} Alternated;
Recombine Surface {6};
