#include <tr1/memory>
#include <Alelib/Mesh>


using namespace std;
using tr1::shared_ptr;

 int main(int argc, char *argv[])
{
  MeshIoMsh msh_reader;
  MeshIoVtk vtk_printer;
  shared_ptr<Mesh> mesh;
  int sdim;
  ECellType ct;

  if (argc<3)
  {
    cout << "usage: msh2vtk meshin.msh meshout.vtk\n";
    exit(1);
  }

  ct = msh_reader.identifiesMshMeshType(argv[1], sdim);

  mesh.reset( Mesh::create(ct,sdim) );
  msh_reader.readFileMsh(argv[1], mesh.get());
  vtk_printer.attachMesh(mesh.get());

  vtk_printer.writeVtk(argv[2]);  
  cout << argv[2] << " done ...\n" << endl;
}

