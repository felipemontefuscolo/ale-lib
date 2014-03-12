//gmsh


Point(1) = {0, 0.2, 0, 1.0};
Point(2) = {0, -0.2, 0, 1.0};
Point(3) = {-0.2, 0, 0, 1.0};
Point(4) = {0.2, 0, 0, 1.0};


Line(1) = {2, 3};
Line(2) = {3, 1};
Line(3) = {1, 4};
Line(4) = {4, 2};
Line(5) = {2, 1};



Point(5) = {0.24, 0.25, 0, 1.0};
Point(6) = {0.23, -0.2, 0, 1.0};
Point(7) = {-0.24, -0.2, 0, 1.0};
Point(8) = {-0.22, 0.26, 0, 1.0};
Line(6) = {1, 5};
Line(7) = {5, 4};
Line(8) = {4, 6};
Line(9) = {6, 2};
Line(10) = {2, 7};
Line(11) = {7, 3};
Line(12) = {3, 8};
Line(13) = {8, 1};
Point(9) = {-0.08, 0.42, 0, 1.0};
Point(10) = {0.1, 0.42, 0, 1.0};
Point(11) = {0.35, 0.1, 0, 1.0};
Point(12) = {0.36, -0.06, 0, 1.0};
Point(13) = {0.1, -0.39, 0, 1.0};
Point(14) = {-0.09, -0.4, 0, 1.0};
Point(15) = {-0.4, -0.07, 0, 1.0};
Point(16) = {-0.39, 0.13, 0, 1.0};
Line(14) = {12, 4};
Line(15) = {11, 12};
Line(16) = {4, 11};
Line(17) = {11, 5};
Line(18) = {12, 6};
Line(19) = {14, 7};
Line(20) = {2, 14};
Line(21) = {2, 13};
Line(22) = {13, 14};
Line(23) = {6, 13};
Line(24) = {3, 15};
Line(25) = {15, 16};
Line(26) = {16, 3};
Line(27) = {16, 8};
Line(28) = {15, 7};
Line(29) = {1, 9};
Line(30) = {9, 10};
Line(31) = {10, 1};
Line(32) = {8, 9};
Line(33) = {5, 10};





Line Loop(34) = {6, 7, -3};
Plane Surface(35) = {34};
Line Loop(36) = {16, 17, 7};
Plane Surface(37) = -{36};
Line Loop(38) = {14, 16, 15};
Plane Surface(39) = {38};
Line Loop(40) = {8, -18, 14};
Plane Surface(41) = -{40};
Line Loop(42) = {3, 4, 5};
Plane Surface(43) = {42};
Line Loop(44) = {9, -4, 8};
Plane Surface(45) = {44};
Line Loop(46) = {23, -21, -9};
Plane Surface(47) = {46};
Line Loop(48) = {20, -22, -21};
Plane Surface(49) = -{48};
Line Loop(50) = {19, -10, 20};
Plane Surface(51) = {50};
Line Loop(52) = {1, -11, -10};
Plane Surface(53) = -{52};
Line Loop(54) = {24, 28, 11};
Plane Surface(55) = -{54};
Line Loop(56) = {26, 24, 25};
Plane Surface(57) = {56};
Line Loop(58) = {27, -12, -26};
Plane Surface(59) = {58};
Line Loop(60) = {13, 29, -32};
Plane Surface(61) = -{60};
Line Loop(62) = {12, 13, -2};
Plane Surface(63) = {62};
Line Loop(64) = {29, 30, 31};
Plane Surface(65) = {64};
Line Loop(66) = {33, 31, 6};
Plane Surface(67) = -{66};
Line Loop(68) = {2, -5, 1};
Plane Surface(69) = {68};

Transfinite Line {32, 13, 29, 30, 31, 6, 33, 7, 3, 5, 2, 12, 27, 26, 25, 24, 28, 11, 1, 19, 10, 20, 22, 21, 9, 23, 4, 8, 18, 14, 16, 15, 17} = 3 Using Progression 1;


Transfinite Surface {35} = {1, 5, 4};
Transfinite Surface {41} = {6, 4, 12};
Transfinite Surface {43} = {2, 1, 4};
Transfinite Surface {65} = {1, 9, 10};
Transfinite Surface {67} = {1, 10, 5};
Transfinite Surface {37} = {4, 5, 11};
Transfinite Surface {39} = {4, 11, 12};
Transfinite Surface {45} = {2, 4, 6};
Transfinite Surface {47} = {13, 2, 6};
Transfinite Surface {49} = {14, 2, 13};
Transfinite Surface {51} = {14, 7, 2};
Transfinite Surface {53} = {7, 3, 2};
Transfinite Surface {55} = {7, 15, 3};
Transfinite Surface {57} = {15, 16, 3};
Transfinite Surface {59} = {3, 16, 8};
Transfinite Surface {69} = {2, 3, 1};
Transfinite Surface {63} = {3, 8, 1};
Transfinite Surface {61} = {1, 8, 9};

// right
Physical Surface(70) = {43, 35, 67, 37, 39, 45, 47, 41, 65};
//left
Physical Surface(71) = {61, 63, 59, 69, 57, 55, 53, 51, 49};
// middle
Physical Line(72) = {5};



