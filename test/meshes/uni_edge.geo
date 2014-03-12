// Gmsh project created on Mon Jul 25 14:35:17 2011
a = 0.1;
Point(1) = {-1, 0, 0, a};
Point(2) = {+1, 0, 0, a};
Line(1) = {1, 2};
