#!python

rank_max = 10


print("\nrow major:\n")

i = 1;
while i<=rank_max: # i = rank

  j = 0;
  name = "i0";
  while j<i-1:
    #name =  'i'+str(j+1) + " + " + 'n'+str(j+1)+'*('+name+')';
    name =  'i'+str(j+1) + " + " + "rdims["+str(j+1)+']*('+name+')';
    j += 1;
    
  print(name)
  i += 1

print("\n==============================================\n")


print("column major:\n")

i = 1;
while i<=rank_max: # i = rank

  j = 1;
  name = 'i'+str(i-1);
  while j<i:
    #name =  'i'+str(i-j-1) + " + " + 'n'+str(i-j-1)+'*('+name+')';
    name =  'i'+str(i-j-1) + " + " + 'rdims['+str(i-j-1)+']*('+name+')';
    j += 1;
    
  print(name)
  i += 1


