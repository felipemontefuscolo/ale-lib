// Gmsh project created on Mon Jul 25 14:49:48 2011
cl1  = 1;
Point(1) = {0, 0, 0, cl1};
Point(2) = {1, 0, 0, cl1};
Point(3) = {1, 1, 0, cl1};
Point(4) = {0, 1, 0, cl1};
Point(5) = {1, 0, 1, cl1};
Point(6) = {1, 1, 1, cl1};
Point(7) = {0, 1, 1, cl1};
Point(8) = {0, 0, 1, cl1};



Line(1) = {2, 1};
Line(2) = {1, 4};
Line(3) = {4, 3};
Line(4) = {3, 2};
Line(5) = {5, 8};
Line(6) = {8, 7};
Line(7) = {7, 6};
Line(8) = {6, 5};
Line(9) = {1, 8};
Line(10) = {7, 4};
Line(11) = {2, 5};
Line(12) = {6, 3};





Line Loop(13) = {1, 2, 3, 4};
Plane Surface(14) = {13};
Line Loop(15) = {5, 6, 7, 8};
Plane Surface(16) = {15};
Line Loop(17) = {9, 6, 10, -2};
Plane Surface(18) = {17};
Line Loop(19) = {11, -8, 12, 4};
Plane Surface(20) = {19};
Line Loop(21) = {9, -5, -11, 1};
Plane Surface(22) = {21};
Line Loop(23) = {10, 3, -12, -7};
Plane Surface(24) = {23};

Surface Loop(25) = {24, 18, 22, 16, 20, 14};



Volume(26) = {25};


Transfinite Line {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12} = 12 Using Progression 1;

Transfinite Surface {16} = {7, 6, 5, 8};
Transfinite Surface {14} = {1, 4, 3, 2};
Transfinite Surface {20} = {6, 3, 2, 5};
Transfinite Surface {18} = {7, 4, 1, 8};
Transfinite Surface {22} = {1, 2, 5, 8};
Transfinite Surface {24} = {7, 4, 3, 6};


Transfinite Volume{26} = {2, 3, 6, 5, 1, 4, 7, 8};



Recombine Surface {20, 18, 14, 16, 24, 22};
