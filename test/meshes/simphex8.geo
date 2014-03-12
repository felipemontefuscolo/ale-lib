// Gmsh project created on Sat Jul 16 22:43:15 2011
Point(1) = {0, 0, 0, 1.0};
Point(2) = {1, 0, 0, 1.0};
Point(3) = {1, 1, 0, 1.0};
Point(4) = {0, 1, 0, 1.0};
Line(1) = {2, 3};
Line(2) = {3, 4};
Line(3) = {4, 1};
Line(4) = {1, 2};
Line Loop(5) = {1, 2, 3, 4};
Plane Surface(6) = {5};
Extrude {0, 0, 1} {
  Surface{6};
}
Transfinite Line {10, 3, 8, 1} = 3 Using Progression 1;
Transfinite Line {18, 2, 14, 9, 22, 4, 13, 11} = 3 Using Progression 1;
Transfinite Surface {28} = {5, 6, 10, 14};
Transfinite Surface {15} = {5, 2, 3, 6};
Transfinite Surface {6} = {2, 3, 4, 1};
Transfinite Surface {23} = {14, 1, 4, 10};
Transfinite Surface {19} = {10, 6, 3, 4};
Transfinite Surface {27} = {14, 5, 2, 1};
Transfinite Volume{1} = {14, 5, 6, 10, 1, 2, 3, 4};
Recombine Surface {28, 15, 6, 23, 19, 27};
