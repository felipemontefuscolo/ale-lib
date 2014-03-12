// Gmsh project created on Mon Jul 25 14:40:07 2011
Point(1) = {-1, -1, 0, 1.0};
Point(2) = {+1, -1, 0, 1.0};
Point(3) = {+1, +1, 0, 1.0};
Point(4) = {-1, +1, 0, 1.0};
Line(1) = {1, 4};
Line(2) = {4, 3};
Line(3) = {3, 2};
Line(4) = {2, 1};

Line Loop(5) = {3, 4, 1, 2};
Plane Surface(6) = {5};
Transfinite Line {1, 4, 3, 2} = 20 Using Progression 1;
Transfinite Surface {6};
Recombine Surface {6};
