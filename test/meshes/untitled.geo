Point(1) = {0, 0, 0, 1.0};
Point(2) = {1, 0, 0, 1.0};
Line(1) = {1, 2};
Point(3) = {0, 1, 0, 1.0};
Point(4) = {1, 1, 0, 1.0};
Line(2) = {2, 4};
Line(3) = {4, 3};
Line(4) = {3, 1};



Line Loop(5) = {4, 1, 2, 3};
Plane Surface(6) = {5};


Physical Point(1) = {1};
Physical Point(2) = {2};
Physical Point(3) = {4};
Physical Point(4) = {3};
Physical Line(5) = {1};
Physical Line(6) = {2};
Physical Line(7) = {3};
Physical Line(8) = {4};
Physical Surface(9) = {6};
Extrude {0, 0, 1} {
  Surface{6};
}
Physical Point(10) = {6};
Physical Point(11) = {10};
Physical Point(12) = {14};
Physical Point(13) = {5};
Physical Line(14) = {12};
Physical Line(15) = {13};
Physical Line(16) = {14};
Physical Line(17) = {11};
Physical Line(18) = {17};
Physical Line(19) = {21};
Physical Line(20) = {25};
Physical Line(21) = {16};
Physical Surface(22) = {22};
Physical Surface(23) = {26};
Physical Surface(24) = {30};
Physical Surface(25) = {18};
Physical Surface(26) = {31};
Physical Volume(27) = {1};
Transfinite Line {1, 21, 12, 17, 16, 25, 14, 3, 2, 13, 11, 4} = 4 Using Progression 1;
Transfinite Surface {22} = {1, 2, 10, 6};
Transfinite Surface {30} = {3, 4, 14, 5};
Transfinite Surface {26} = {2, 4, 14, 10};
Transfinite Surface {18} = {1, 3, 5, 6};
Transfinite Surface {31} = {6, 10, 14, 5};
Transfinite Surface {6} = {1, 2, 4, 3};
Transfinite Volume{1} = {1, 2, 10, 6, 3, 4, 14, 5};




Recombine Surface {22, 26, 30, 18, 31, 6};






